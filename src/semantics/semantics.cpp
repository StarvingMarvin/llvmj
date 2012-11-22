
#include "semantics.h"
#include <iostream>

using namespace mj;
using std::cout;

const Type& visitChild(AstWalker &walker, nodeiterator &ni) {
    walker.visit(*ni);
    const Type *t = getNodeData<const Type>(*ni);
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
        std::cerr << "ERROR! Types " << l << " and " << r << " not compatible!" << std::endl;
    }
}

void VarDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * ident = tokenText(*b);
    const Variable *v = symbols.resolveVariable(ident);
    if (v != NULL) {
        setType(walker, v->type());
    } else {
        std::cerr << "ERROR! Var expected, got " << ident << std::endl;
    }
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
    if (mjInt == l && mjInt == r) {
        setType(walker, mjInt);
    } else {
        std::cerr << "ERROR! Both arguments should be int, instead got " 
            << l << " and " << r << std::endl;
    }
}

void UnOpVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    const Type &mjInt = *symbols.resolveType("int");
    const Type &l = visitChild(walker, ni);

    if (mjInt != l) {
        std::cerr << "ERROR! int expected, got " << l << std::endl;
    } else {
        setType(walker, mjInt);
    }
}

void MethodVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char *typeName = tokenText(*ni);
    const Type &returnType = *symbols.resolveType(typeName);
    ni++;
    char *methodName = tokenText(*ni);
#ifdef DEBUG
    cout << "Method return type: " << returnType << std::endl;
    cout << "Method name: " << methodName << std::endl;
#endif

    MethodArguments &arguments = symbols.enterMethodArgumentsScope();
    ni++;
    walker.visit(*ni);
    symbols.leaveScope();

    ni++;
    symbols.enterMethodScope(methodName, returnType, arguments);
    visitChildren(walker, ni);
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
    if (t != NULL) {
        ni++;
        char *varName = tokenText(*ni);
        symbols.defineVariable(varName, *t);
    } else {
        std::cerr << "ERROR! type expected, got " << typeName << "!" << std::endl;
    }
}

void ArrVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char *typeName = tokenText(*ni);
    const Type *t = symbols.resolveType(typeName);
    ni++;
    char *varName = tokenText(*ni);

    if (t == NULL) {
        std::cerr << "ERROR! type expected, got " << typeName << "!" << std::endl;
    } else {
        symbols.defineArray(varName, *t);
    }
}

void PrintVisitor::operator()(AstWalker &walker) const {
    const Type &mjInt = *symbols.resolveType("int");
    const Type &mjChar = *symbols.resolveType("char");
    nodeiterator ni = walker.firstChild();
    const Type &t = visitChild(walker, ni);
    if ((t!=mjInt) && (t!=mjChar)) {
        std::cerr << "ERROR! int or char expected, got " << t << std::endl;
    }
}

void ReadVisitor::operator()(AstWalker &walker) const {
    const Type mjInt = *symbols.resolveType("int");
    const Type mjChar = *symbols.resolveType("char");
    nodeiterator ni = walker.firstChild();
    const Type &t = visitChild(walker, ni);
    if ((t!=mjInt) && (t!=mjChar)) {
        std::cerr << "ERROR! int or char expected, got " << t << std::endl;
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
        std::cerr << "ERROR! Invalid argument types: (";
        for (ArgumentTypes::iterator it = argumentTypes.begin();
                it != argumentTypes.end(); it++) {
            std::cerr << *(*it) << ", ";
        }
        std::cerr << "). Expected " << mt->arguments().typeSignature() <<std::endl;
    }

#ifdef DEBUG
    cout << "Method type: " << *mt << std::endl;
    cout << "Return type: " << mt->returnType() << std::endl;
#endif
    setType(walker, mt->returnType());
}

void FieldDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    ni++;
    const Variable *var = symbols.resolveVariable(name);
    if (var == NULL) {
        //
    }
    const Class &clazz = dynamic_cast<const Class&>(var->type());
    char* fieldName = tokenText(*ni);
    const Symbol *fieldSymbol = clazz.scope().resolve(fieldName);
    const Variable *field = dynamic_cast<const Variable*>(fieldSymbol);
    if (field == NULL) {
        //
    }

    setType(walker, field->type());
}

void ArrDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);

    const Variable *arrVar = symbols.resolveVariable(name);
    if (arrVar == NULL) {
        //
    }

    const ArrayType &arr = dynamic_cast<const ArrayType&>(arrVar->type());
    setType(walker, arr.valueType());
}

void NewVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    const Class *clazz = symbols.resolveClass(name);
    if (clazz == NULL) {
        std::cerr << "ERROR! Expected class name, got " << name << "!" << std::endl;
    }
    setType(walker, *clazz);
}

void NewArrVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    const Type *t = symbols.resolveType(name);
    if (t == NULL) {
        std::cerr << "ERROR! Type is NULL!" << std::endl;
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
        //
    }

    const MethodType &mt = mainFunc->methodType();

    if(!mt.arguments().matchArguments(ArgumentTypes())
            && mt.returnType() != VOID_TYPE) {
        //
    }

    symbols.leaveScope();
}

void mj::checkSemantics(AST ast, Symbols &symbolsTable) {
    AstWalker walker(ast,  VisitChildren());

    walker.addVisitor(VAR_DES, VarDesVisitor(symbolsTable));
    walker.addVisitor(FIELD_DES, FieldDesVisitor(symbolsTable));
    walker.addVisitor(ARR_DES, ArrDesVisitor(symbolsTable));
    walker.addVisitor(LIT_INT, IntLiteralVisitor(symbolsTable));
    walker.addVisitor(LIT_CHAR, CharLiteralVisitor(symbolsTable));
    walker.addVisitor(CALL, CallVisitor(symbolsTable));

    IntOpVisitor iov(symbolsTable);
    walker.addVisitor(PLUS, iov);
    walker.addVisitor(MINUS, iov);
    walker.addVisitor(MUL, iov);
    walker.addVisitor(DIV, iov);
    walker.addVisitor(MOD, iov);

    walker.addVisitor(UNARY_MINUS, UnOpVisitor(symbolsTable));

    walker.addVisitor(VAR, VarVisitor(symbolsTable));
    walker.addVisitor(CONST, VarVisitor(symbolsTable));
    walker.addVisitor(ARR, ArrVisitor(symbolsTable));
    walker.addVisitor(FN, MethodVisitor(symbolsTable));
    walker.addVisitor(CLASS, ClassVisitor(symbolsTable));
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

    walker.addVisitor(PRINT, PrintVisitor(symbolsTable));
    walker.addVisitor(READ, ReadVisitor(symbolsTable));

    walker.addVisitor(PROGRAM, ProgramVisitor(symbolsTable));

    walker.walkTree();
}
