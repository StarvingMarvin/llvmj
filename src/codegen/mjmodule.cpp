
#include "llvmjConfig.h"

#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <stdexcept>
#include <vector>

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

CodegenVisitor::CodegenVisitor(llvm::Module &module, const Symbols &symbols):
    _module(module),
    _symbols(symbols)
{
}

namespace mj {
namespace codegen {


static Value* visitChild(AstWalker &walker, nodeiterator &ni) {
    walker.visit(*ni);
    Value *v = getNodeData<Value>(*ni);
    if (v == NULL) {
        // wut?
    }
    ni++;
    return v;
}

}
}


void VarDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * ident = tokenText(*b);
    const NamedValue *v = symbols().resolveNamedValue(ident);
}

void MethodVisitor::operator()(AstWalker &walker) const {

}

void IntLiteralVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * val = tokenText(*b);
    walker.setData(ConstantInt::get(llvm::getGlobalContext(), 
                                APInt(32, val, 10)));
}

void BinopVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* lhs = visitChild(walker, ni);
    Value* rhs = visitChild(walker, ni);
    Value* result = op(lhs, rhs);
    walker.setData(result);
}

Value* AddVisitor::op(Value* lhs, Value* rhs) const {
   return builder.CreateAdd(lhs, rhs, "addtmp");
}

Value* SubVisitor::op(Value* lhs, Value* rhs) const {
   return builder.CreateSub(lhs, rhs, "subtmp");
}

void NegOpVisitor::operator()(AstWalker &walker) const {
    
}

void AssignVisitor::operator()(AstWalker &walker) const {
    
}

Values::Values(llvm::Module *module, const Symbols &symbols) {
    const GlobalScope &global = symbols.globalScope();
    const Program *program = global.program();

    type_iterator type_it = global.typesBegin();
    type_iterator type_end = global.typesEnd();
    for(; type_it < type_end; type_it++) {
        const Type *t = *type_it;
    }

    constant_iterator const_it = global.constantBegin();
    constant_iterator const_end = global.constantEnd();
    for(;const_it < const_end; const_it++) {
        const Constant *c = *const_it;
    }
}

llvm::Value* Values::value(const string &name) const {

    Value* ret = NULL;

    if (currentScope == NULL) {
        LocalValues::const_iterator scope_it = localValues.find(*currentScope);
        if (scope_it == localValues.end()) {
            throw new runtime_error("Invalid scope name: " + (*currentScope));
        }
        ValueTable *local = scope_it->second;
        ValueTable::const_iterator local_it = local->find(name);
        if (local_it != local->end()) {
            ret = local_it->second;
        }
    }

    if (ret != NULL) {
        ValueTable::const_iterator global_it = globalValues.find(name);
        if( global_it != globalValues.end() ) {
            ret = global_it->second;
        }
    }

    return ret;
}

llvm::Type* Values::type(const string &name) const {
    TypeTable::const_iterator it = types.find(name);
    if (it != types.end()) {
        return it->second;
    }
    return NULL;
}

void Values::enterScope(const string &name) {
    currentScope = &name;
}

void Values::leaveScope() {
    currentScope = NULL;
}

MjModule::MjModule(AST ast, const Symbols &symbols):
    _ast(ast), 
    _symbols(symbols),
    _module(symbols.globalScope().program()->name(), llvm::getGlobalContext()),
    values(&_module, _symbols)
{
    walkTree();
}

void MjModule::walkTree() {
    VisitChildren defVisitor;

    VarDesVisitor vdv(_module, _symbols);
    MethodVisitor methodv(_module, _symbols);
    IntLiteralVisitor ilv(_module, _symbols);
    AddVisitor addv(_module, _symbols);
    SubVisitor subv(_module, _symbols);

    AstWalker walker(defVisitor);

    walker.addVisitor(VAR_DES, vdv);
    //walker.addVisitor(FIELD_DES, FieldDesVisitor(symbolsTable));
    //walker.addVisitor(ARR_DES, ArrDesVisitor(symbolsTable));
    walker.addVisitor(LIT_INT, ilv);
    //walker.addVisitor(LIT_CHAR, CharLiteralVisitor(symbolsTable));
    //walker.addVisitor(CALL, CallVisitor(symbolsTable));

    //walker.addVisitor(WHILE, LoopVisitor(symbolsTable));
    //walker.addVisitor(IF, LoopVisitor(symbolsTable));
    //walker.addVisitor(BREAK, UnexpectedBreakVisitor(symbolsTable));

    //IntOpVisitor iov(symbolsTable);
    walker.addVisitor(PLUS, addv);
    walker.addVisitor(MINUS, subv);
    //walker.addVisitor(MUL, iov);
    //walker.addVisitor(DIV, iov);
    //walker.addVisitor(MOD, iov);

    walker.addVisitor(DEFFN, methodv);
    //walker.addVisitor(NEW, NewVisitor(symbolsTable));
    //walker.addVisitor(NEW_ARR, NewArrVisitor(symbolsTable));

    //CheckCompatibleVisitor ccv(symbolsTable);
    //walker.addVisitor(SET, ccv);
    //walker.addVisitor(EQL, ccv);
    //walker.addVisitor(NEQ, ccv);
    //walker.addVisitor(GRT, ccv);
    //walker.addVisitor(GRE, ccv);
    //walker.addVisitor(LST, ccv);
    //walker.addVisitor(LSE, ccv);

    //UnOpVisitor uov(symbolsTable);
    //walker.addVisitor(INC, uov);
    //walker.addVisitor(DEC, uov);
    //walker.addVisitor(UNARY_MINUS, uov);

    //walker.addVisitor(PRINT, PrintVisitor(symbolsTable));
    //walker.addVisitor(READ, ReadVisitor(symbolsTable));

    //walker.addVisitor(PROGRAM, ProgramVisitor(symbolsTable));

    walker.visit(_ast);
}
