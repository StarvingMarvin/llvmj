
#include "semantics.h"

using namespace mj;



Type* childType(AstWalker *walker, nodeiterator ni) {
    walker->visit(*ni);
    Type *t = getNodeData<Type>(*ni);
    ni++;
    return t;
}

void BinOpVisitor::operator()(AstWalker *walker) {
    nodeiterator b = walker->firstChild();
    Type *l = childType(walker, b);
    Type *r = childType(walker, b);
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
}

void FuncVisitor::operator()(AstWalker *walker) {
}

void ClassVisitor::operator()(AstWalker *walker) {
}

void VarVisitor::operator()(AstWalker *walker) {
}

void PrintVisitor::operator()(AstWalker *walker) {
    const Type mjInt = *symbols->resolveType("int");
    const Type mjChar = *symbols->resolveType("char");
    nodeiterator ni = walker->firstChild();
    Type t = *childType(walker, ni);
    if ((t!=mjInt) && (t!=mjChar)) {
        // screem
    }
    if (ni != walker->lastChild()) {
        t = *childType(walker, ni);
        if (t!=mjInt) {
            //screem
        }
    }
}

void ReadVisitor::operator()(AstWalker *walker) {
    const Type mjInt = *symbols->resolveType("int");
    const Type mjChar = *symbols->resolveType("char");
    nodeiterator ni = walker->firstChild();
    Type t = *childType(walker, ni);
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
    AstWalker walker(ast);

    Symbols *symbolsTable = new Symbols();


    return symbolsTable;
}
