
#include "semantics.h"

using namespace mj;


SetVisitor::SetVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void SetVisitor::operator()(AstWalker *walker) {
}

GetVisitor::GetVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void GetVisitor::operator()(AstWalker *walker) {
}

IntLiteralVisitor::IntLiteralVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void IntLiteralVisitor::operator()(AstWalker *walker) {
    const Symbol *s = symbols->resolve("int");
    const Type *t = dynamic_cast<const Type*>(s);
    walker->setData(const_cast<Type*>(t));
}

CharLiteralVisitor::CharLiteralVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void CharLiteralVisitor::operator()(AstWalker *walker) {
    const Symbol *s = symbols->resolve("char");
    const Type *t = dynamic_cast<const Type*>(s);
    walker->setData(const_cast<Type*>(t));
}

BoolOpVisitor::BoolOpVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void BoolOpVisitor::operator()(AstWalker *walker) {
}

IntOpVisitor::IntOpVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void IntOpVisitor::operator()(AstWalker *walker) {
}

DefVisitor::DefVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void DefVisitor::operator()(AstWalker *walker) {
}

FuncVisitor::FuncVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void FuncVisitor::operator()(AstWalker *walker) {
}

ClassVisitor::ClassVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void ClassVisitor::operator()(AstWalker *walker) {
}

VarVisitor::VarVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void VarVisitor::operator()(AstWalker *walker) {
}

PrintVisitor::PrintVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void PrintVisitor::operator()(AstWalker *walker) {
}

ReadVisitor::ReadVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void ReadVisitor::operator()(AstWalker *walker) {
}

LoopVisitor::LoopVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void LoopVisitor::operator()(AstWalker *walker) {
}

CallVisitor::CallVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void CallVisitor::operator()(AstWalker *walker) {
}

DotVisitor::DotVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void DotVisitor::operator()(AstWalker *walker) {
}

IndexVisitor::IndexVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void IndexVisitor::operator()(AstWalker *walker) {
}

NewVisitor::NewVisitor(Symbols *symbolsTable):
    symbols(symbolsTable)
{
}

void NewVisitor::operator()(AstWalker *walker) {
}

Symbols* mj::checkSemantics(AST ast) {
    AstWalker walker(ast);

    Symbols *symbolsTable = new Symbols();


    return symbolsTable;
}
