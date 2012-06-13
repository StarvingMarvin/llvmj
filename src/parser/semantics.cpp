
#include "semantics.h"

using namespace mj;


void SetVisitor::operator()(AstWalker *walker) {

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

void BoolOpVisitor::operator()(AstWalker *walker) {

}

void IntOpVisitor::operator()(AstWalker *walker) {
}

void RelOpVisitor::operator()(AstWalker *walker) {
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
