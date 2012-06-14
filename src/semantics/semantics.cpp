
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

void GetVisitor::operator()(AstWalker *walker) {
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
    const Symbol *s = symbols->resolve("int");
    const Type *t = dynamic_cast<const Type*>(s);
    walker->setData(const_cast<Type*>(t));
}

void CharLiteralVisitor::operator()(AstWalker *walker) {
    const Symbol *s = symbols->resolve("char");
    const Type *t = dynamic_cast<const Type*>(s);
    walker->setData(const_cast<Type*>(t));
}

bool BoolOpVisitor::check(Type *l, Type *r) {
   return true;
}

bool IntOpVisitor::check(Type *l, Type *r) {
    const Symbol *s = symbols->resolve("int");
    const Type *mjInt = dynamic_cast<const Type*>(s);
    return *mjInt == *l && *mjInt == *r;
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
}

void ReadVisitor::operator()(AstWalker *walker) {
}

void LoopVisitor::operator()(AstWalker *walker) {
}

void CallVisitor::operator()(AstWalker *walker) {
}

void DotVisitor::operator()(AstWalker *walker) {
}

void IndexVisitor::operator()(AstWalker *walker) {
}

void NewVisitor::operator()(AstWalker *walker) {
}

Symbols* mj::checkSemantics(AST ast) {
    AstWalker walker(ast);

    Symbols *symbolsTable = new Symbols();


    return symbolsTable;
}
