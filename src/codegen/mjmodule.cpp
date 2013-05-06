
#include "llvmjConfig.h"

#include <string>
#include <iostream>
#include <stack>
#include <vector>
#include <map>

#include <llvm/DerivedTypes.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

#include "parser/parser.h"
#include "semantics/semantics.h"
#include "codegen/mjmodule.h"


using namespace std;
using namespace mj;
using namespace mj::codegen;

using llvm::Module;
using llvm::IRBuilder;
using llvm::ConstantInt;
using llvm::APInt;
using llvm::Value;

IRBuilder<> CodegenVisitor::builder(llvm::getGlobalContext());

CodegenVisitor::CodegenVisitor(llvm::Module *module, Symbols &symbols): 
    _module(module),
    _symbols(symbols)
{
}

static const Value* visitChild(AstWalker &walker, nodeiterator &ni) {
    walker.visit(*ni);
    const Value *v = getNodeData<const Value>(*ni);
    if (v == NULL) {
        // wut?
    }
    ni++;
    return v;
}

void setValue(AstWalker &walker, const Value* value) {
   walker.setData(const_cast<Value*>(value));
}

void VarDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * ident = tokenText(*b);
    const NamedValue *v = symbols().resolveNamedValue(ident);
    
}

void IntLiteralVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * val = tokenText(*b);
    walker.setData(ConstantInt::get(llvm::getGlobalContext(), 
                                APInt(32, val, 10)));
}

void BinopVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    const Value* lhs = visitChild(walker, ni);
    const Value* rhs = visitChild(walker, ni);
    const Value* result = op(lhs, rhs);
    setValue(walker, result);
}

const Value* AddVisitor::op(const Value* lhs, const Value* rhs) const {
   return builder.CreateAdd(lhs, rhs, "addtmp");
}

void SubVisitor::operator()(AstWalker &walker) const {
   return builder.CreateSub(lhs, rhs, "subtmp");
}

void NegOpVisitor::operator()(AstWalker &walker) const {
    
}

void AssignVisitor::operator()(AstWalker &walker) const {
    
}

void Values::define(const string &name, Value *v) {
    values[name] = v;
}

Value* Values::resolve(const string &name) {
    ValueTable::const_iterator it = values.find(name);
    if( it != values.end() ) {
        return it->second;
    }

    if (_parent != NULL) {
        return _parent->resolve(name);
    }

    return NULL;
}

Values::~Values() {
    ValueTable::iterator it = values.begin();
    for (; it != values.end(); it++) {
        delete it->second;
    }
}

MjModule::MjModule(AST ast, Symbols &s):
    _ast(ast), 
    symbols(s), 
    _module(s.globalScope().program()->name(), llvm::getGlobalContext()),
    values(Values(NULL))
{
}

void MjModule::init() {
    const GlobalScope &gs = symbols.globalScope();
    const Program *p = gs.program();
    p->name();
}

