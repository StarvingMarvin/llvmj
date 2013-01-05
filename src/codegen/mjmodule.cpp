
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

void VarDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * ident = tokenText(*b);
    const Variable *v = symbols().resolveVariable(ident);
    
}

void IntLiteralVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * val = tokenText(*b);
    walker.setData(ConstantInt::get(llvm::getGlobalContext(), 
                                APInt(32, val, 10)));
}

void AddVisitor::operator()(AstWalker &walker) const {
    
}

void SubVisitor::operator()(AstWalker &walker) const {
    
}

void NegOpVisitor::operator()(AstWalker &walker) const {
    
}

void AssignVisitor::operator()(AstWalker &walker) const {
    
}

MjModule::MjModule(AST ast)
{
    //module = new Module(programName, getGlobalContext());

}



MjModule::MjModule(const char* name) {

}

void MjModule::writeAsm(std::ostream &out) {
//  verifyModule(*module, PrintMessageAction);
//  PassManager PM;
//  raw_os_ostream ros(out);
//  PM.add(createPrintModulePass(&ros));
//  PM.run(*module);
}

void MjModule::writeBc(std::ostream &out) {
//  verifyModule(*module, PrintMessageAction);
//  PassManager PM;
//  raw_os_ostream ros(out);
//  PM.add(createPrintModulePass(&ros));
//  PM.run(*module);
}

MjModule::~MjModule() {
}


