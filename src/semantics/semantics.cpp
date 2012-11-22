
#include "semantics.h"
#include <iostream>

using namespace mj;
using std::cout;

template<class T>
T* visitChild(AstWalker &walker, nodeiterator &ni) {
    walker.visit(*ni);
    T *t = getNodeData<T>(*ni);
    ni++;
    return t;
}

void visitChildren(AstWalker &walker, nodeiterator &ni) {
    for(; ni < walker.lastChild(); ni++) {
        walker.visit(*ni);
    }
}

void CheckCompatibleVisitor::operator()(AstWalker &walker) {
    nodeiterator b = walker.firstChild();
    Type &l = *visitChild<Type>(walker, b);
    Type &r = *visitChild<Type>(walker, b);
    
    if (!l.compatible(r)) {
        std::cerr << "ERROR! Types " << l << " and " << r << " not compatible!" << std::endl;
    }
}

void VarDesVisitor::operator()(AstWalker &walker) {
    nodeiterator b = walker.firstChild();
    char * ident = tokenText(*b);
#ifdef DEBUG
    cout << "Var name: " << ident << std::endl;
#endif
    const Symbol *s = symbols.resolve(ident);
    const Variable *v = dynamic_cast<const Variable*>(s);
    if (v != NULL) {
        const Type &t = v->type();
#ifdef DEBUG
        cout << "Var type: " << t.name() << std::endl;
#endif

        walker.setData(const_cast<Type*>(&t));
    } else {
        std::cerr << "ERROR! Var expected, got " << ident << std::endl;
    }
}

void IntLiteralVisitor::operator()(AstWalker &walker) {
    const Type *t = symbols.resolveType("int");
    walker.setData(const_cast<Type*>(t));
}

void CharLiteralVisitor::operator()(AstWalker &walker) {
    const Type *t = symbols.resolveType("char");
    walker.setData(const_cast<Type*>(t));
}

void IntOpVisitor::operator()(AstWalker &walker) {
    const Type &mjInt = *symbols.resolveType("int");
    nodeiterator b = walker.firstChild();
    Type &l = *visitChild<Type>(walker, b);
    Type &r = *visitChild<Type>(walker, b);
    if (mjInt == l && mjInt == r) {
        walker.setData(const_cast<Type*>(&mjInt));
    } else {
        std::cerr << "ERROR! Both arguments should be int, instead got " 
            << l << " and " << r << std::endl;
    }
}

void UnOpVisitor::operator()(AstWalker &walker) {
    nodeiterator ni = walker.firstChild();
    const Type &mjInt = *symbols.resolveType("int");
    Type &l = *visitChild<Type>(walker, ni);

    if (mjInt != l) {
        std::cerr << "ERROR! int expected, got " << l << std::endl;
    }
    walker.setData(const_cast<Type *>(&mjInt));
}

void DefVisitor::operator()(AstWalker &walker) {
    nodeiterator ni = walker.firstChild();
    Symbol *s = visitChild<Symbol>(walker, ni);
#ifdef DEBUG
    cout << "Defining: " << *s << std::endl;
#endif
    symbols.define(s);
}

void MethodVisitor::operator()(AstWalker &walker) {
    nodeiterator ni = walker.firstChild();
    char *typeName = tokenText(*ni);
#ifdef DEBUG
    cout << "Method return type name: " << typeName << std::endl;
#endif
    const Type &returnType = *symbols.resolveType(typeName);
    ni++;
    char *methodName = tokenText(*ni);
#ifdef DEBUG
    cout << "Method return type: " << returnType << std::endl;
    cout << "Method name: " << methodName << std::endl;
#endif

    MethodArguments *arguments = symbols.enterMethodArgumentsScope();
    ni++;
    visitChild<void>(walker, ni);
    symbols.leaveScope();

    Method *m = symbols.enterMethodScope(methodName, returnType, arguments);
    visitChildren(walker, ni);
    symbols.leaveScope();
#ifdef DEBUG
    cout << "Method type: " << m->type() << std::endl;
#endif

    walker.setData(m);
}

void ClassVisitor::operator()(AstWalker &walker) {
    nodeiterator ni = walker.firstChild();
    char *className = tokenText(*ni);

    Class *c = symbols.enterClassScope(className);
    visitChildren(walker, ni);
    symbols.leaveScope();

    walker.setData(c);
}

void VarVisitor::operator()(AstWalker &walker) {
    nodeiterator ni = walker.firstChild();
    char *typeName = tokenText(*ni);
    const Type *t = symbols.resolveType(typeName);
    if (t != NULL) {
        ni++;
        char *varName = tokenText(*ni);
        Variable *v = new Variable(varName, *t);
        walker.setData(v);
    } else {
        std::cerr << "ERROR! type expected, got " << typeName << "!" << std::endl;
    }
}

void ArrVisitor::operator()(AstWalker &walker) {
    nodeiterator ni = walker.firstChild();
    char *typeName = tokenText(*ni);
    const Type &t = *symbols.resolveType(typeName);
    ni++;
    char *varName = tokenText(*ni);

    const ArrayType *at = dynamic_cast<const ArrayType*>(symbols.resolve(ArrayType(t).name()));

    if (! at) {
        at = new const ArrayType(t);
        symbols.define(const_cast<ArrayType*>(at));
    }
    Variable *v = new Variable(varName, *at);
    walker.setData(v);
}

void PrintVisitor::operator()(AstWalker &walker) {
    const Type &mjInt = *symbols.resolveType("int");
    const Type &mjChar = *symbols.resolveType("char");
    nodeiterator ni = walker.firstChild();
    Type &t = *visitChild<Type>(walker, ni);
    if ((t!=mjInt) && (t!=mjChar)) {
        std::cerr << "ERROR! int or char expected, got " << t << std::endl;
    }
}

void ReadVisitor::operator()(AstWalker &walker) {
    const Type mjInt = *symbols.resolveType("int");
    const Type mjChar = *symbols.resolveType("char");
    nodeiterator ni = walker.firstChild();
    Type &t = *visitChild<Type>(walker, ni);
    if ((t!=mjInt) && (t!=mjChar)) {
        std::cerr << "ERROR! int or char expected, got " << t << std::endl;
    }
}

void CallVisitor::operator()(AstWalker &walker) {
    nodeiterator ni = walker.firstChild();

    const MethodType *mt = visitChild<const MethodType>(walker, ni);
    ArgumentTypes argumentTypes;

    while(ni < walker.lastChild()) {
        argumentTypes.push_back(visitChild<const Type>(walker, ni));
    }

    if (!mt->arguments()->matchArguments(argumentTypes)) {
        std::cerr << "ERROR! Invalid argument types: (";
        for (ArgumentTypes::iterator it = argumentTypes.begin();
                it != argumentTypes.end(); it++) {
            std::cerr << *(*it) << ", ";
        }
        std::cerr << "). Expected " << mt->arguments()->typeSignature() <<std::endl;
    }

#ifdef DEBUG
    cout << "Method type: " << *mt << std::endl;
    cout << "Return type: " << mt->returnType() << std::endl;
#endif
    walker.setData(const_cast<Type*>(&mt->returnType()));
}

void FieldDesVisitor::operator()(AstWalker &walker) {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    ni++;
    const Symbol *varSymbol = symbols.resolve(name);
    const Variable *var = dynamic_cast<const Variable*>(varSymbol);
    if (var == NULL) {
        //
    }
    const Class &clazz = dynamic_cast<const Class&>(var->type());
    char* fieldName = tokenText(*ni);
    const Symbol *fieldSymbol = clazz.scope()->resolve(fieldName);
    const Variable *field = dynamic_cast<const Variable*>(fieldSymbol);
    if (field == NULL) {
        //
    }

    const Type *fieldType = &field->type();

    walker.setData(const_cast<Type*>(fieldType));

}

void ArrDesVisitor::operator()(AstWalker &walker) {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    const Symbol *arrSymbol = symbols.resolve(name);
    if (arrSymbol == NULL) {
        //
    }

    const Variable *arrVar = dynamic_cast<const Variable*>(arrSymbol);
    const ArrayType &arr = dynamic_cast<const ArrayType&>(arrVar->type());

    walker.setData(const_cast<Type*>(&arr.valueType()));
}

void NewVisitor::operator()(AstWalker &walker) {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    const Symbol *classSymbol = symbols.resolve(name);
    const Class *clazz = dynamic_cast<const Class*>(classSymbol);
    if (clazz == NULL) {
        std::cerr << "ERROR! Class is NULL!" << std::endl;
    }
    walker.setData(const_cast<Class*>(clazz));

}

void NewArrVisitor::operator()(AstWalker &walker) {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    const Symbol *typeSymbol = symbols.resolve(name);
    const Type *t = dynamic_cast<const Type*>(typeSymbol);
    if (t == NULL) {
        std::cerr << "ERROR! Type is NULL!" << std::endl;
    }
    walker.setData(new ArrayType(*t));
}

void ProgramVisitor::operator()(AstWalker &walker) {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    Program *p = symbols.enterProgramScope(name);
    visitChildren(walker, ni);
    symbols.leaveScope();
    symbols.define(p);
}

Symbols* mj::checkSemantics(AST ast) {
    AstWalker walker(ast, new VisitChildren());

    Symbols &symbolsTable = *(new Symbols());

    walker.addVisitor(VAR_DES, new VarDesVisitor(symbolsTable));
    walker.addVisitor(FIELD_DES, new FieldDesVisitor(symbolsTable));
    walker.addVisitor(ARR_DES, new ArrDesVisitor(symbolsTable));
    walker.addVisitor(LIT_INT, new IntLiteralVisitor(symbolsTable));
    walker.addVisitor(LIT_CHAR, new CharLiteralVisitor(symbolsTable));
    walker.addVisitor(CALL, new CallVisitor(symbolsTable));

    IntOpVisitor *iov = new IntOpVisitor(symbolsTable);
    walker.addVisitor(PLUS, iov);
    walker.addVisitor(MINUS, iov);
    walker.addVisitor(MUL, iov);
    walker.addVisitor(DIV, iov);
    walker.addVisitor(MOD, iov);

    walker.addVisitor(UNARY_MINUS, new UnOpVisitor(symbolsTable));

    walker.addVisitor(DEF, new DefVisitor(symbolsTable));
    walker.addVisitor(VAR, new VarVisitor(symbolsTable));
    walker.addVisitor(CONST, new VarVisitor(symbolsTable));
    walker.addVisitor(ARR, new ArrVisitor(symbolsTable));
    walker.addVisitor(FN, new MethodVisitor(symbolsTable));
    walker.addVisitor(CLASS, new ClassVisitor(symbolsTable));
    walker.addVisitor(NEW, new NewVisitor(symbolsTable));
    walker.addVisitor(NEW_ARR, new NewArrVisitor(symbolsTable));

    CheckCompatibleVisitor *ccv = new CheckCompatibleVisitor(symbolsTable);
    walker.addVisitor(SET, ccv);
    walker.addVisitor(EQL, ccv);
    walker.addVisitor(NEQ, ccv);
    walker.addVisitor(GRT, ccv);
    walker.addVisitor(GRE, ccv);
    walker.addVisitor(LST, ccv);
    walker.addVisitor(LSE, ccv);

    UnOpVisitor *uov = new UnOpVisitor(symbolsTable);
    walker.addVisitor(INC, uov);
    walker.addVisitor(DEC, uov);

    walker.addVisitor(PRINT, new PrintVisitor(symbolsTable));
    walker.addVisitor(READ, new ReadVisitor(symbolsTable));

    walker.addVisitor(PROGRAM, new ProgramVisitor(symbolsTable));

    walker.walkTree();

    return &symbolsTable;
}
