
#include <string>
#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <stdexcept>
#include <sstream>

#include "semantics.h"

using namespace mj;
using namespace mj::semantics;
using namespace std;

bool SemanticNodeVisitor::dirty = false;
const Type UNKNOWN_TYPE("mj.unknown");

static const Type& visitChild(AstWalker &walker, nodeiterator &ni) {
    walker.visit(*ni);
    const Type *t = getNodeData<const Type>(*ni);
    if (t == NULL) {
        t = &UNKNOWN_TYPE;
    }
    ni++;
    return *t;
}

void setType(AstWalker &walker, const Type& type) {
   walker.setData(const_cast<Type*>(&type));
}

void visitChildren(AstWalker &walker, nodeiterator &ni) {
    for(; ni < walker.lastChild(); ni++) {
        walker.visit(*ni);
    }
}

void CheckCompatibleVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    const Type &l = visitChild(walker, b);
    const Type &r = visitChild(walker, b);
    
    if (!l.compatible(r)) {
        cerr << "ERROR! Types " << l << " and " << r << " not compatible at ";
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
    }
}

void VarDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * ident = tokenText(*b);
    const NamedValue *v = symbols.resolveNamedValue(ident);
    if (v == NULL) {
        cerr << "ERROR! Unknown variable '" << ident << "' at ";
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }

    if (dynamic_cast<const Constant*>(v)) {
        cerr << "ERROR: '" << ident << "' is a constant and l-value is expected at ";
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }

    setType(walker, v->type());
}

void SetVisitor::operator()(AstWalker &walker) const {
    const NodeVisitor &nv = walker.getVisitor(VAR_DES);
    walker.addVisitor(VAR_DES, VarDesVisitor(symbols));
    CheckCompatibleVisitor::operator()(walker);
    walker.addVisitor(VAR_DES, nv);
}

void NamedValueVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * ident = tokenText(*b);
    const NamedValue *v = symbols.resolveNamedValue(ident);
    if (v == NULL) {
        cerr << "ERROR! Unknown variable '" << ident << "' at ";
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }

    setType(walker, v->type());
}

void IntLiteralVisitor::operator()(AstWalker &walker) const {
    const Type &t = *symbols.resolveType("int");
    setType(walker, t);
}

void CharLiteralVisitor::operator()(AstWalker &walker) const {
    const Type &t = *symbols.resolveType("char");
    setType(walker, t);
}

void IntOpVisitor::operator()(AstWalker &walker) const {
    const Type &mjInt = *symbols.resolveType("int");
    nodeiterator b = walker.firstChild();
    const Type &l = visitChild(walker, b);
    const Type &r = visitChild(walker, b);
    
    if (mjInt != l || mjInt != r) {
        cerr << "ERROR! Both arguments should be int, instead got '" 
            << l << "' and '" << r << "'" << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }

    setType(walker, mjInt);
}

void UnOpVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    const Type &mjInt = *symbols.resolveType("int");
    const Type &l = visitChild(walker, ni);

    if (mjInt != l) {
        cerr << "ERROR! int expected, got " << l << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    } 

    setType(walker, mjInt);
}

void MethodVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char *typeName = tokenText(*ni);
    const Type *returnType = symbols.resolveType(typeName);

    if (returnType == NULL) {
        cerr << "ERROR! Unknown type: " << typeName << "!" << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }

    ni++;
    char *methodName = tokenText(*ni);

    MethodArguments &arguments = symbols.enterMethodArgumentsScope();
    ni++;
    walker.visit(*ni);
    symbols.leaveScope();

    ni++;
    symbols.enterMethodScope(methodName, *returnType, arguments);
    const NodeVisitor &nv = walker.getVisitor(RETURN);
    walker.addVisitor(RETURN, ReturnVisitor(symbols, *returnType));
    visitChildren(walker, ni);
    walker.addVisitor(RETURN, nv);
    symbols.leaveScope();
}

void ClassVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char *className = tokenText(*ni);

    symbols.enterClassScope(className);
    visitChildren(walker, ni);
    symbols.leaveScope();
}

void VarVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char *typeName = tokenText(*ni);
    const Type *t = symbols.resolveType(typeName);
    if (t == NULL) {
        cerr << "ERROR! Unknown type: " << typeName << "!" << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }

    ni++;
    char *varName = tokenText(*ni);
    symbols.defineNamedValue(varName, *t);
}

void ConstVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char *typeName = tokenText(*ni);
    const Type *t = symbols.resolveType(typeName);
    if (t == NULL) {
        cerr << "ERROR! Unknown type: " << typeName << "!" << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }
    
    ni++;
    char *varName = tokenText(*ni);

    ni++;
    char *valStr = tokenText(*ni);
    int val = 0;

    if (t->name() == "int") {
        istringstream(valStr) >> val; 
    } else {
        val = valStr[0];
    }

    symbols.defineConstant(varName, *t, val);
}

void ArrVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char *typeName = tokenText(*ni);
    const Type *t = symbols.resolveType(typeName);
    ni++;
    char *varName = tokenText(*ni);

    if (t == NULL) {
        cerr << "ERROR! Unknown type: " << typeName << "!" << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }

    symbols.defineArray(varName, *t);
}

void PrintVisitor::operator()(AstWalker &walker) const {
    const Type &mjInt = *symbols.resolveType("int");
    const Type &mjChar = *symbols.resolveType("char");
    nodeiterator ni = walker.firstChild();
    const Type &t = visitChild(walker, ni);
    if ((t!=mjInt) && (t!=mjChar)) {
        cerr << "ERROR! int or char expected, got " << t << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
    }
}

void ReadVisitor::operator()(AstWalker &walker) const {
    const Type mjInt = *symbols.resolveType("int");
    const Type mjChar = *symbols.resolveType("char");
    nodeiterator ni = walker.firstChild();
    const Type &t = visitChild(walker, ni);
    if ((t!=mjInt) && (t!=mjChar)) {
        cerr << "ERROR! int or char expected, got " << t << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
    }
}

void LoopVisitor::operator()(AstWalker &walker) const {
    const NodeVisitor &nv = walker.getVisitor(BREAK);

    // NodeVisitor is noop
    walker.addVisitor(BREAK, NodeVisitor());
    nodeiterator ni = walker.firstChild();
    visitChildren(walker, ni);
    walker.addVisitor(BREAK, nv);
}

void UnexpectedBreakVisitor::operator()(AstWalker &walker) const {
    cerr << "ERROR! break only valid within loops." << endl;
    walker.printPosition(cerr) << "!" << endl;
    setDirty();
}


void ReturnVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    if (ni == walker.lastChild()) {
        if (type != VOID_TYPE) {
            cerr << "ERROR! Method must return value of type " << type << endl;
            walker.printPosition(cerr) << "!" << endl;
            setDirty();
        }
    } else {
        const Type &rt = visitChild(walker, ni);
        if (!type.compatible(rt)) {
            cerr << "ERROR! Method must return value of type " << type << endl;
            walker.printPosition(cerr) << "!" << endl;
            setDirty();
        }
    }
}

void CallVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();

    const Type &t = visitChild(walker, ni);
    const MethodType *mt = dynamic_cast<const MethodType*>(&t);
    ArgumentTypes argumentTypes;

    while(ni < walker.lastChild()) {
        argumentTypes.push_back(&visitChild(walker, ni));
    }

    if (!mt->arguments().matchArguments(argumentTypes)) {
        cerr << "ERROR! Invalid argument types: (";
        for (ArgumentTypes::iterator it = argumentTypes.begin();
                it != argumentTypes.end(); it++) {
            cerr << *(*it) << ", ";
        }
        cerr << "). Expected " << mt->arguments().typeSignature() <<endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }

#ifdef DEBUG
    cout << "Method type: " << *mt << endl;
#endif
    setType(walker, mt->returnType());
}

void FieldDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    ni++;
    const NamedValue *var = symbols.resolveNamedValue(name);
    if (var == NULL) {
        cerr << "ERROR! Unknown variable: " << name << "!" << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }
    const Class &clazz = dynamic_cast<const Class&>(var->type());
    char* fieldName = tokenText(*ni);
    const Symbol *fieldSymbol = clazz.scope().resolve(fieldName);
    const NamedValue *field = dynamic_cast<const NamedValue*>(fieldSymbol);
    if (field == NULL) {
        cerr << "ERROR! Unknown field: " << fieldName << " in class " 
            << name << "!" << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }

    setType(walker, field->type());
}

void ArrDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);

    const NamedValue *arrVar = symbols.resolveNamedValue(name);
    if (arrVar == NULL) {
        cerr << "ERROR! Unknown variable: " << name << "!" << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }

    const ArrayType &arr = dynamic_cast<const ArrayType&>(arrVar->type());
    setType(walker, arr.valueType());
}

void NewVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    const Class *clazz = symbols.resolveClass(name);
    if (clazz == NULL) {
        cerr << "ERROR! Unknown class: " << name << "!" << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }
    setType(walker, *clazz);
}

void NewArrVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    const Type *t = symbols.resolveType(name);
    if (t == NULL) {
        cerr << "ERROR! Unknown type: " << name << "!" << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
        return;
    }

    walker.setData(new ArrayType(*t));
}

void ProgramVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    symbols.enterProgramScope(name);
    visitChildren(walker, ni);
    
    // check main
    const Method *mainFunc = symbols.resolveMethod("main");
    if (mainFunc == NULL) {
        cerr << "ERROR! Method 'main' must be defined!" << endl;
        walker.printPosition(cerr)<< "!" << endl;
        setDirty();
    } else {

        const MethodType &mt = mainFunc->methodType();
        if(!mt.arguments().matchArguments(ArgumentTypes())
                && mt.returnType() != VOID_TYPE) {
            cerr << "ERROR! Method 'main' doesn't accept any parameters!" << endl;
            walker.printPosition(cerr)<< "!" << endl;
            setDirty();
        }
    }

    symbols.leaveScope();
}

void mj::checkSemantics(AST ast, Symbols &symbolsTable) {
    AstWalker walker(ast,  VisitChildren());

    walker.addVisitor(VAR_DES, NamedValueVisitor(symbolsTable));
    walker.addVisitor(FIELD_DES, FieldDesVisitor(symbolsTable));
    walker.addVisitor(ARR_DES, ArrDesVisitor(symbolsTable));
    walker.addVisitor(LIT_INT, IntLiteralVisitor(symbolsTable));
    walker.addVisitor(LIT_CHAR, CharLiteralVisitor(symbolsTable));
    walker.addVisitor(CALL, CallVisitor(symbolsTable));

    walker.addVisitor(WHILE, LoopVisitor(symbolsTable));
    walker.addVisitor(BREAK, UnexpectedBreakVisitor(symbolsTable));

    IntOpVisitor iov(symbolsTable);
    walker.addVisitor(PLUS, iov);
    walker.addVisitor(MINUS, iov);
    walker.addVisitor(MUL, iov);
    walker.addVisitor(DIV, iov);
    walker.addVisitor(MOD, iov);

    walker.addVisitor(DEFVAR, VarVisitor(symbolsTable));
    walker.addVisitor(DEFCONST, ConstVisitor(symbolsTable));
    walker.addVisitor(DEFARR, ArrVisitor(symbolsTable));
    walker.addVisitor(DEFFN, MethodVisitor(symbolsTable));
    walker.addVisitor(DEFCLASS, ClassVisitor(symbolsTable));
    walker.addVisitor(NEW, NewVisitor(symbolsTable));
    walker.addVisitor(NEW_ARR, NewArrVisitor(symbolsTable));

    CheckCompatibleVisitor ccv(symbolsTable);
    walker.addVisitor(SET, ccv);
    walker.addVisitor(EQL, ccv);
    walker.addVisitor(NEQ, ccv);
    walker.addVisitor(GRT, ccv);
    walker.addVisitor(GRE, ccv);
    walker.addVisitor(LST, ccv);
    walker.addVisitor(LSE, ccv);

    UnOpVisitor uov(symbolsTable);
    walker.addVisitor(INC, uov);
    walker.addVisitor(DEC, uov);
    walker.addVisitor(UNARY_MINUS, uov);

    walker.addVisitor(PRINT, PrintVisitor(symbolsTable));
    walker.addVisitor(READ, ReadVisitor(symbolsTable));

    walker.addVisitor(PROGRAM, ProgramVisitor(symbolsTable));

    walker.walkTree();

    if (SemanticNodeVisitor::dirty) {
        throw runtime_error("Semantic check failed!");
    }
}
