
#include "semantics.h"

using namespace mj;


template<class T>
T* visitChild(AstWalker *walker, nodeiterator ni) {
    walker->visit(*ni);
    T *t = getNodeData<T>(*ni);
    ni++;
    return t;
}

void BinOpVisitor::operator()(AstWalker *walker) {
    nodeiterator b = walker->firstChild();
    Type *l = visitChild<Type>(walker, b);
    Type *r = visitChild<Type>(walker, b);
    if (!check(l, r)) {
        //scream
    }
}

bool SetVisitor::check(Type *l, Type *r) {
    return l->compatible(*r);
}

void VarDesVisitor::operator()(AstWalker *walker) {
    nodeiterator b = walker->firstChild();
    char * ident = tokenText(*b);
    const Symbol *s = symbols->resolve(ident);
    const Variable *v = dynamic_cast<const Variable*>(s);
    if (v != NULL) {
        const Type &t = v->type();
        walker->setData(const_cast<Type*>(&t));
    }
}

void IntLiteralVisitor::operator()(AstWalker *walker) {
    const Type *t = symbols->resolveType("int");
    walker->setData(const_cast<Type*>(t));
}

void CharLiteralVisitor::operator()(AstWalker *walker) {
    const Type *t = symbols->resolveType("char");
    walker->setData(const_cast<Type*>(t));
}

bool BoolOpVisitor::check(Type *l, Type *r) {
   return true;
}

bool IntOpVisitor::check(Type *l, Type *r) {
    const Type mjInt = *symbols->resolveType("int");
    return mjInt == *l && mjInt == *r;
}

bool RelOpVisitor::check(Type *l, Type *r) {
    return l->compatible(*r);
}

void DefVisitor::operator()(AstWalker *walker) {
    nodeiterator ni = walker->firstChild();
    Symbol *s = visitChild<Symbol>(walker, ni);
    symbols->define(s);
}

void MethodVisitor::operator()(AstWalker *walker) {
    nodeiterator ni = walker->firstChild();
    char *typeName = tokenText(*ni);
    const Type t = *symbols->resolveType(typeName);
    ni++;
    char *methodName = tokenText(*ni);
    Method *m = new Method(methodName, t);
    walker->setData(m);
}

void ClassVisitor::operator()(AstWalker *walker) {
    nodeiterator ni = walker->firstChild();
    char *className = tokenText(*ni);
    Class *c = new Class(className);
    walker->setData(c);
}

void VarVisitor::operator()(AstWalker *walker) {
    nodeiterator ni = walker->firstChild();
    char *typeName = tokenText(*ni);
    Type t = *symbols->resolveType(typeName);
    ni++;
    char *varName = tokenText(*ni);
    Variable *v = new Variable(varName, t);
    walker->setData(v);
}

void ArrVisitor::operator()(AstWalker *walker) {
    nodeiterator ni = walker->firstChild();
    char *typeName = tokenText(*ni);
    const Type t = *symbols->resolveType(typeName);
    ni++;
    char *varName = tokenText(*ni);
    Variable *v = new Variable(varName, ArrayType(t));
    walker->setData(v);
}

void PrintVisitor::operator()(AstWalker *walker) {
    const Type mjInt = *symbols->resolveType("int");
    const Type mjChar = *symbols->resolveType("char");
    nodeiterator ni = walker->firstChild();
    Type t = *visitChild<Type>(walker, ni);
    if ((t!=mjInt) && (t!=mjChar)) {
        // screem
    }
    if (ni != walker->lastChild()) {
        t = *visitChild<Type>(walker, ni);
        if (t!=mjInt) {
            //screem
        }
    }
}

void ReadVisitor::operator()(AstWalker *walker) {
    const Type mjInt = *symbols->resolveType("int");
    const Type mjChar = *symbols->resolveType("char");
    nodeiterator ni = walker->firstChild();
    Type t = *visitChild<Type>(walker, ni);
    if ((t!=mjInt) && (t!=mjChar)) {
        // screem
    }
}

void LoopVisitor::operator()(AstWalker *walker) {
}

void CallVisitor::operator()(AstWalker *walker) {
    nodeiterator ni = walker->firstChild();
    char* name = tokenText(*ni);
    const Symbol *methodSymbol = symbols->resolve(name);
    const Method *method = dynamic_cast<const Method*>(methodSymbol);
    if (method == NULL) {
        //
    }
    // check arguments
    Type *t = new Type(method->returnType());
    walker->setData(t);
}

void FieldDesVisitor::operator()(AstWalker *walker) {
}

void ArrDesVisitor::operator()(AstWalker *walker) {
}

void NewVisitor::operator()(AstWalker *walker) {
    nodeiterator ni = walker->firstChild();
    char* name = tokenText(*ni);
    const Symbol *classSymbol = symbols->resolve(name);
    const Class *clazz = dynamic_cast<const Class*>(classSymbol);
    if (clazz == NULL) {
        //
    }
    // check arguments
    walker->setData(const_cast<Class*>(clazz));

}

Symbols* mj::checkSemantics(AST ast) {
    AstWalker walker(ast, new VisitChildren());

    Symbols *symbolsTable = new Symbols();
    walker.addVisitor(SET, new SetVisitor(symbolsTable));
    walker.addVisitor(VAR_DES, new VarDesVisitor(symbolsTable));
    walker.addVisitor(FIELD_DES, new FieldDesVisitor(symbolsTable));
    walker.addVisitor(ARR_DES, new ArrDesVisitor(symbolsTable));
    walker.addVisitor(LIT_INT, new IntLiteralVisitor(symbolsTable));
    walker.addVisitor(LIT_CHAR, new SetVisitor(symbolsTable));
    walker.addVisitor(CALL, new CallVisitor(symbolsTable));

    IntOpVisitor *iov = new IntOpVisitor(symbolsTable);
    walker.addVisitor(PLUS, iov);
    walker.addVisitor(MINUS, iov);
    walker.addVisitor(MUL, iov);
    walker.addVisitor(DEC, iov);
    walker.addVisitor(MOD, iov);
    
    walker.addVisitor(DEF, new DefVisitor(symbolsTable));
    walker.addVisitor(VAR, new VarVisitor(symbolsTable));
    walker.addVisitor(ARR, new ArrVisitor(symbolsTable));
    walker.addVisitor(FN, new MethodVisitor(symbolsTable));
    walker.addVisitor(CLASS, new ClassVisitor(symbolsTable));

    //walker.addVisitor(INC, new SetVisitor(symbolsTable));
    //walker.addVisitor(DEC, new SetVisitor(symbolsTable));
   
    BoolOpVisitor *bov = new BoolOpVisitor(symbolsTable);
    walker.addVisitor(OR, bov);
    walker.addVisitor(AND, bov);
    
    RelOpVisitor *rov = new RelOpVisitor(symbolsTable);
    walker.addVisitor(EQL, rov);
    walker.addVisitor(NEQ, rov);
    walker.addVisitor(GRT, rov);
    walker.addVisitor(GRE, rov);
    walker.addVisitor(LST, rov);
    walker.addVisitor(LSE, rov);

    return symbolsTable;
}
