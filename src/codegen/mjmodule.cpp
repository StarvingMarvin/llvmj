
#include "llvmjConfig.h"

#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <stdexcept>
#include <vector>

#include <llvm/Analysis/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>


#include "parser/parser.h"
#include "semantics/symbols.h"
#include "semantics/semantics.h"
#include "codegen/values.h"
#include "codegen/mjmodule.h"

using namespace std;
using namespace mj;
using namespace mj::codegen;

using llvm::APInt;
using llvm::ArrayType;
using llvm::BasicBlock;
using llvm::ConstantInt;
using llvm::dyn_cast;
using llvm::EngineBuilder;
using llvm::ExecutionEngine;
using llvm::Function;
using llvm::FunctionType;
using llvm::GlobalValue;
using llvm::GlobalVariable;
using llvm::IntegerType;
using llvm::isa;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::Module;
using llvm::PointerType;
using llvm::StructType;
using llvm::DataLayout;
using llvm::TypeBuilder;
using llvm::Value;

IRBuilder<> CodegenVisitor::builder(llvm::getGlobalContext());

CodegenVisitor::CodegenVisitor(llvm::Module &module, Values &values):
    _module(module),
    _values(values)
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
    walker.setData(values().value(ident));
}

void MethodVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    ni++; // skipping type name
    char * ident = tokenText(*ni);

    const Program *program = symbols.globalScope().program();
    const Symbol* symMeth = program->scope().resolve(ident);
    const Method* meth = dynamic_cast<const Method*>(symMeth);

    llvm::Type *t = values().type(meth->methodType().name());
    FunctionType *ft = dyn_cast<FunctionType>(t);

    string name = program->name() + "::" + meth->name();
    Function *f = Function::Create(ft, Function::InternalLinkage, name, &module());
    //values().define(meth->name(), f);

    BasicBlock *bb = BasicBlock::Create(module().getContext(), "entry", f);
    builder.SetInsertPoint(bb);

    Value *ret = NULL;
    llvm::Type *rt = ft->getReturnType();
    if (!rt->isVoidTy()) {
        ret = builder.CreateAlloca(rt);
    }

    ValueTable local;

    const MethodType &methodType = meth->methodType();
    arguments_iterator var_it = methodType.arguments().argumentsBegin();

    Function::arg_iterator arg_it = f->arg_begin();

    //cout << "Function: " << f->getName().str() << "(";

    for (; var_it != methodType.arguments().argumentsEnd(); var_it++) {
        const NamedValue *nv = *var_it;

        Value *arg = arg_it++;
        llvm::Type *argType = arg->getType();
        arg->setName(nv->name());

        //cout.flush();

        //llvm::raw_os_ostream rout(cout);
        //argType->print(rout);
        //rout.flush();
        //cout << " " << nv->name() << ",";

        Value *argVar = builder.CreateAlloca(argType, 0, nv->name());
        builder.CreateStore(arg, argVar);
        local[nv->name()] = argVar;
    }

    //cout << ")" << endl;

    Scope &methodScope = meth->scope();
    Scope::iterator sym_it = methodScope.begin();

    for (; sym_it != methodScope.end(); sym_it++) {
        const Symbol &s = *sym_it;
        const NamedValue &nv = dynamic_cast<const NamedValue&>(s);
        llvm::Type *t = values().type(nv.type().name());
        local[nv.name()] = builder.CreateAlloca(t, 0, nv.name());
    }

    values().enterFunction(meth->name(), f, &local);

    while ( ni != walker.lastChild()) {
        visitChild(walker, ni);
    }

    bb = builder.GetInsertBlock();

    if (!bb->getTerminator()) {
        if (rt->isVoidTy()) {
            builder.CreateRetVoid();
        } else {
            Value *rv = builder.CreateLoad(ret);
            builder.CreateRet(rv);
        }
    }

    values().leaveFunction();

    llvm::verifyFunction(*f);

}

void CallVisitor::operator ()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();

    Value *funcVal = visitChild(walker, ni);

    vector<Value*> args;

    Function *func = dyn_cast<Function>(funcVal);
    FunctionType *ft = func->getFunctionType();

    FunctionType::param_iterator pi = ft->param_begin();

    while(ni < walker.lastChild()) {
        Value *arg = visitChild(walker, ni);
        llvm::Type *at = *pi;
        args.push_back(cast(arg, at));
        pi++;
    }

    string retName = "";

    if (!ft->getReturnType()->isVoidTy()) {
        retName = "call_tmp";
    }

    walker.setData(builder.CreateCall(func, args, retName));

}

void CharLiteralVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * val = tokenText(*b);
    walker.setData(ConstantInt::get(values().type("char"), val[0]));
}

void IntLiteralVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * val = tokenText(*b);
    walker.setData(values().constInt(val));
}

void BinopVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();

    Value* lhs = visitChild(walker, ni);
    Value* rhs = visitChild(walker, ni);
    Value* casted = cast(rhs, lhs->getType());
    Value* result = op(lhs, casted);
    walker.setData(result);
}

Value* AddVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateAdd(lhs, rhs, "add_tmp");
}

Value* SubVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateSub(lhs, rhs, "sub_tmp");
}

Value* MulVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateMul(lhs, rhs, "mul_tmp");
}

Value* DivVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateSDiv(lhs, rhs, "div_tmp");
}

Value* ModVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateSRem(lhs, rhs, "mod_tmp");
}

Value* EqlVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateICmpEQ(lhs, rhs, "eq_tmp");
}

Value* NeqVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateICmpNE(lhs, rhs, "ne_tmp");
}

Value* GrtVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateICmpSGT(lhs, rhs, "gt_tmp");
}

Value* GreVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateICmpSGE(lhs, rhs, "ge_tmp");
}

Value* LstVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateICmpSLT(lhs, rhs, "lt_tmp");
}

Value* LseVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateICmpSLE(lhs, rhs, "le_tmp");
}

Value* AndVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateAnd(lhs, rhs, "and_tmp");
}

Value* OrVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateOr(lhs, rhs, "or_tmp");
}

void AssignVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* var = visitChild(walker, ni);
    Value* rhs = visitChild(walker, ni);
    llvm::Type *vt = var->getType();
    PointerType *pt = dyn_cast<PointerType>(vt);
    llvm::Type *t = pt->getElementType();
    Value *casted = cast(rhs, t);
    walker.setData(builder.CreateStore(casted, var));
}

void IncVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* var = visitChild(walker, ni);
    Value *val = builder.CreateLoad(var, false, "inc_tmp");
    Value *inc = builder.CreateAdd(val, values().constInt(1), "add_tmp");
    builder.CreateStore(inc, var);
    walker.setData(val);
}

void DecVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* var = visitChild(walker, ni);
    Value *val = builder.CreateLoad(var, false, "dec_tmp");
    Value *dec = builder.CreateSub(val, values().constInt(1), "sub_tmp");
    builder.CreateStore(dec, var);
    walker.setData(val);
}

void NegOpVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* val = visitChild(walker, ni);
    //Value *val = builder.CreateLoad(var, false, "neg_tmp");
    walker.setData(builder.CreateNeg(val, "neg_tmp"));
}


void WhileVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();


    Function *f = builder.GetInsertBlock()->getParent();
    LLVMContext &ctx = module().getContext();

    BasicBlock *condBlock = BasicBlock::Create(ctx, "while");
    builder.CreateBr(condBlock);
    f->getBasicBlockList().push_back(condBlock);
    builder.SetInsertPoint(condBlock);

    Value *cond = visitChild(walker, ni);

    BasicBlock *loopBlock = BasicBlock::Create(ctx, "loop");
    BasicBlock *mergeBlock = BasicBlock::Create(ctx, "whilecont");
    builder.CreateCondBr(cond, loopBlock, mergeBlock);

    values().breakPoints().push_back(mergeBlock);

    f->getBasicBlockList().push_back(loopBlock);
    builder.SetInsertPoint(loopBlock);

    visitChild(walker, ni);

    loopBlock = builder.GetInsertBlock();

    if (!loopBlock->getTerminator()) {
        builder.CreateBr(condBlock);
    }

    values().breakPoints().pop_back();

    f->getBasicBlockList().push_back(mergeBlock);
    builder.SetInsertPoint(mergeBlock);

}

void IfVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value *cond = visitChild(walker, ni);

    Function *f = builder.GetInsertBlock()->getParent();
    LLVMContext &ctx = module().getContext();

    BasicBlock *thenBlock = BasicBlock::Create(ctx, "then", f);
    BasicBlock *elseBlock = BasicBlock::Create(ctx, "else");
    BasicBlock *mergeBlock = BasicBlock::Create(ctx, "ifcont");

    builder.CreateCondBr(cond, thenBlock, elseBlock);

    builder.SetInsertPoint(thenBlock);

    visitChild(walker, ni);

    thenBlock = builder.GetInsertBlock();
    if (!thenBlock->getTerminator()) {
        builder.CreateBr(mergeBlock);
    }

    f->getBasicBlockList().push_back(elseBlock);
    builder.SetInsertPoint(elseBlock);

    if (ni != walker.lastChild()) {
        visitChild(walker, ni);
    }

    elseBlock = builder.GetInsertBlock();
    if (!elseBlock->getTerminator()) {
        builder.CreateBr(mergeBlock);
    }

    f->getBasicBlockList().push_back(mergeBlock);
    builder.SetInsertPoint(mergeBlock);

}

void BreakVisitor::operator()(AstWalker &walker) const {
    BasicBlock *breakPoint = values().breakPoints().back();
    builder.CreateBr(breakPoint);
}

void RetVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    if (ni == walker.lastChild()) {
        builder.CreateRetVoid();
    } else {
        Value *ret = visitChild(walker, ni);
        builder.CreateRet(cast(ret, values().returnType()));
    }

}

void FieldDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    ni++;
    Value *var = values().value(name);

    char* fieldName = tokenText(*ni);

    PointerType *sppt = dyn_cast<PointerType>(var->getType());
    llvm::Type *sppt_et = sppt->getElementType();
    PointerType *spt = dyn_cast<PointerType>(sppt_et);
    StructType *st = dyn_cast<StructType>(spt->getElementType());

    int idx = values().index(st->getName().str(), fieldName);
    Value *structPtrVal = builder.CreateLoad(var, false, "struct_deref");

    walker.setData(structPtrField(structPtrVal, idx));
}

void ArrDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    Value *var = values().value(name);

    ni++;
    Value *index = visitChild(walker, ni);

    vector<Value*> idx;
    idx.push_back(values().constInt(0));
    idx.push_back(values().constInt(1));
    Value *arrayDataPtr = builder.CreateGEP(var, idx, "array_data_ptr");

    Value *arrayData = builder.CreateLoad(arrayDataPtr, "array_data");

    Value *result = builder.CreateGEP(arrayData, index, "elem_ptr");
    walker.setData(result);
}

void NewVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* typeName = tokenText(*ni);
    llvm::Type *ptype = values().type(typeName);
    llvm::Type *stype = ptype->getContainedType(0);
    StructType *s = dyn_cast<StructType>(stype);
    size_t size = sizeOf(s);
    Value* voidPtr = callMalloc(size);
    Value* structPtr = builder.CreateBitCast(voidPtr, ptype, s->getName() + "_ptr");
    walker.setData(structPtr);
}

void NewArrVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* typeName = tokenText(*ni);
    ni++;
    Value *arrSize = visitChild(walker, ni);

    llvm::Type *type = values().type(typeName);
    llvm::Type *ptype = PointerType::get(type, 0);
    llvm::Type *atype = arrayType(ptype);
    llvm::Type *patype = PointerType::get(atype, 0);

    uint64_t typeSize = sizeOf(type);
    Value *typeSizeVal = ConstantInt::get(values().type("mj.size_t"), typeSize, true);

    Value *dataSize = builder.CreateMul(typeSizeVal, arrSize, "data_size_tmp");

    Value *voidPtr = callMalloc(dataSize);
    Value *arrayData = builder.CreateBitCast(voidPtr, ptype, "array_data_ptr");

    uint64_t structSize = sizeOf(atype);
    voidPtr = callMalloc(structSize);
    Value *arrayStructPtr = builder.CreateBitCast(voidPtr, patype, "array_ptr");

    Value *aSizePtr = structPtrField(arrayStructPtr, 0);
    Value *arrSizeL = builder.CreateSExtOrBitCast(arrSize, values().type("mj.size_t"));
    builder.CreateStore(arrSizeL, aSizePtr);

    Value *aDataPtr = structPtrField(arrayStructPtr, 1);
    builder.CreateStore(arrayData, aDataPtr);

    walker.setData(builder.CreateLoad(arrayStructPtr, "array"));

}

void DerefVisitor::operator ()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value *ref = visitChild(walker, ni);
    Value *val = builder.CreateLoad(ref, false, ref->getName());
    walker.setData(val);
}


void PrintVisitor::operator ()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value *val = visitChild(walker, ni);
    IntegerType *it = dyn_cast<IntegerType>(val->getType());
    if (it->getBitWidth() == 8) {
        Value *ext = builder.CreateZExt(val, values().type("int"));
        Value* putchar = module().getFunction("putchar");
        builder.CreateCall(putchar, ext);
    } else {
        Value* printf = module().getFunction("printf");
        Value *fmtVar = values().value("mj.dec_fmt");
        vector<Value*> idx;
        idx.push_back(ConstantInt::get(values().type("int"), 0));
        idx.push_back(ConstantInt::get(values().type("int"), 0));
        Value *fmt = builder.CreateGEP(fmtVar, idx, "fmt");
        vector<Value*> args;
        args.push_back(fmt);
        args.push_back(val);
        builder.CreateCall(printf, args);
    }
}

void ReadVisitor::operator ()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value *var = visitChild(walker, ni);
    PointerType *pt = dyn_cast<PointerType>(var->getType());
    IntegerType *it = dyn_cast<IntegerType>(pt->getElementType());
    if (it->getBitWidth() == 8) {
        Value *getchar = module().getFunction("getchar");
        Value *char_tmp = builder.CreateCall(getchar, "char_tmp");
        Value *c = builder.CreateTrunc(char_tmp, values().type("char"));
        builder.CreateStore(c, var);
    } else {
        Value* scanf = module().getFunction("__isoc99_scanf");
        Value *fmtVar = values().value("mj.dec_fmt");
        vector<Value*> idx;
        idx.push_back(ConstantInt::get(values().type("int"), 0));
        idx.push_back(ConstantInt::get(values().type("int"), 0));
        Value *fmt = builder.CreateGEP(fmtVar, idx, "fmt");
        vector<Value*> args;
        args.push_back(fmt);
        args.push_back(var);
        builder.CreateCall(scanf, args);
    }
}

uint64_t CodegenVisitor::sizeOf(llvm::Type* t) const {
    DataLayout dl (&_module);
    return dl.getTypeAllocSize(t);
}

llvm::Type* CodegenVisitor::arrayType(llvm::Type *ptype) const {
    LLVMContext &ctx = _module.getContext();
    vector<llvm::Type*> fields;
    fields.push_back(IntegerType::get(ctx, 64));
    fields.push_back(ptype);
    return StructType::get(ctx, fields);
}

llvm::Value* CodegenVisitor::callMalloc(uint64_t size) const {
    return callMalloc(ConstantInt::get(_module.getContext(), APInt(64, size, false)));
}

llvm::Value* CodegenVisitor::callMalloc(Value *size) const {
    vector<Value*> args;
    args.push_back(size);
    Value* vMalloc = _module.getFunction("malloc");
    return builder.CreateCall(vMalloc, args, "voidptr");
}

llvm::Value* CodegenVisitor::structPtrField(llvm::Value *structPtr, int idx) const {
    Value *idxVal = values().constInt(idx);
    Value *ptrDeref = values().constInt(0);
    vector<Value*> indexes;
    indexes.push_back(ptrDeref);
    indexes.push_back(idxVal);
    return builder.CreateGEP(structPtr, indexes);
}

llvm::Value* CodegenVisitor::cast(llvm::Value* val, llvm::Type* type) const {
    if (val->getType() == type) {
        return val;
    }
    return builder.CreateBitCast(val, type, "cast_tmp");
}



MjModule::MjModule(AST ast, const Symbols &symbols):
    _ast(ast),
    _symbols(symbols),
    _module(symbols.globalScope().program()->name(), llvm::getGlobalContext()),
    values(&_module, _symbols)
{
    makeStdLib();
    walkTree();
    makeMain();

}

void MjModule::makeStdLib() {

    LLVMContext &ctx = _module.getContext();

    // ord(char)
    FunctionType *ord_type = TypeBuilder<llvm::types::i<32>(llvm::types::i<8>), true>::get(ctx);
    Function* func_ord = Function::Create(
     /*Type=*/ord_type,
     /*Linkage=*/GlobalValue::ExternalLinkage,
     /*Name=*/"ord", &_module);

    BasicBlock *bb = BasicBlock::Create(_module.getContext(), "entry", func_ord);
    IRBuilder<> builder(ctx);
    builder.SetInsertPoint(bb);

    values.enterFunction("ord", func_ord, NULL);

    Function::arg_iterator arg_it = func_ord->arg_begin();

    Value *argC = arg_it++;

    //Value *c = builder.CreateLoad(argC, false, "c");

    Value *ret = builder.CreateSExt(argC, values.type("int"));
    builder.CreateRet(ret);

    values.leaveFunction();


    // chr(int)
    FunctionType *chr_type = TypeBuilder<llvm::types::i<8>(llvm::types::i<32>), true>::get(_module.getContext());
    Function* func_chr = Function::Create(
     /*Type=*/chr_type,
     /*Linkage=*/GlobalValue::ExternalLinkage,
     /*Name=*/"chr", &_module);

    bb = BasicBlock::Create(_module.getContext(), "entry", func_chr);
    builder.SetInsertPoint(bb);

    values.enterFunction("chr", func_chr, NULL);

    arg_it = func_chr->arg_begin();

    Value *argI = arg_it++;

    //Value *i = builder.CreateLoad(argI, false, "i");

    ret = builder.CreateTrunc(argI, values.type("char"));
    builder.CreateRet(ret);
    values.leaveFunction();


    // len(arr)
    llvm::Type* arrType = values.type("mj.array");
    vector<llvm::Type*> lenArgs;
    lenArgs.push_back(arrType);
    FunctionType *len_type = FunctionType::get(values.type("int"), lenArgs, false);
    Function* func_len = Function::Create(
     /*Type=*/len_type,
     /*Linkage=*/GlobalValue::ExternalLinkage,
     /*Name=*/"len", &_module);

    bb = BasicBlock::Create(ctx, "entry", func_len);
    builder.SetInsertPoint(bb);

    values.enterFunction("len", func_len, NULL);

    arg_it = func_len->arg_begin();

    Value *argA = arg_it++;

    Value *arr = builder.CreateAlloca(arrType);
    builder.CreateStore(argA, arr);

    vector<Value*> idx;
    idx.push_back(values.constInt(0));
    idx.push_back(values.constInt(0));

    Value *lenPtr = builder.CreateGEP(arr, idx, "len_ptr");
    Value *len = builder.CreateLoad(lenPtr, false, "len");

    builder.CreateRet(len);
    values.leaveFunction();

    // print(int)


}

void MjModule::makeMain() {
    FunctionType *main_type = TypeBuilder<llvm::types::i<32>(), true>::get(_module.getContext());
    Function* func_main = Function::Create(
     /*Type=*/main_type,
     /*Linkage=*/GlobalValue::ExternalLinkage,
     /*Name=*/"main", &_module);
    func_main->setCallingConv(llvm::CallingConv::C);

    BasicBlock *bb = BasicBlock::Create(_module.getContext(), "entry", func_main);
    IRBuilder<> builder(_module.getContext());
    builder.SetInsertPoint(bb);

    Value *mjMain = values.value("main");
    builder.CreateCall(mjMain);
    builder.CreateRet(values.constInt(0));

}

void MjModule::run(int argc, const char** argv) {
    string err;
    llvm::InitializeNativeTarget();
    ExecutionEngine *engine = EngineBuilder(&_module).setErrorStr(&err).create();
    cerr << err << endl;
    int (*fp)() = (int (*)())engine->getPointerToFunction(_module.getFunction("main"));
    fp();
}

void MjModule::walkTree() {
    VisitChildren defVisitor;

    AddVisitor addv(_module, values);
    AssignVisitor assignv(_module, values);
    FieldDesVisitor fdv(_module, values);
    ArrDesVisitor adv(_module, values);
    IntLiteralVisitor ilv(_module, values);
    CharLiteralVisitor clv(_module, values);
    MethodVisitor methodv(_module, values, _symbols);
    CallVisitor callv(_module, values);
    NewVisitor newv(_module, values);
    NewArrVisitor nav(_module, values);

    SubVisitor subv(_module, values);
    MulVisitor mulv(_module, values);
    DivVisitor divv(_module, values);
    ModVisitor modv(_module, values);

    EqlVisitor eqlv(_module, values);
    NeqVisitor neqv(_module, values);
    GrtVisitor grtv(_module, values);
    GreVisitor grev(_module, values);
    LstVisitor lstv(_module, values);
    LseVisitor lsev(_module, values);

    AndVisitor andv(_module, values);
    AndVisitor orv(_module, values);

    WhileVisitor whilev(_module, values);
    IfVisitor ifv(_module, values);
    BreakVisitor breakv(_module, values);

    NegOpVisitor negv(_module, values);
    IncVisitor incv(_module, values);
    DecVisitor decv(_module, values);

    VarDesVisitor vdv(_module, values);
    DerefVisitor derefv(_module, values);
    RetVisitor retv(_module, values);

    PrintVisitor printv(_module, values);
    ReadVisitor readv(_module, values);

    AstWalker walker(defVisitor);

    walker.addVisitor(VAR_DES, vdv);
    walker.addVisitor(FIELD_DES, fdv);
    walker.addVisitor(ARR_DES, adv);
    walker.addVisitor(LIT_INT, ilv);
    walker.addVisitor(LIT_CHAR, clv);
    walker.addVisitor(CALL, callv);

    walker.addVisitor(WHILE, whilev);
    walker.addVisitor(IF, ifv);
    walker.addVisitor(BREAK, breakv);

    walker.addVisitor(PLUS, addv);
    walker.addVisitor(MINUS, subv);
    walker.addVisitor(MUL, mulv);
    walker.addVisitor(DIV, divv);
    walker.addVisitor(MOD, modv);

    walker.addVisitor(DEFFN, methodv);
    walker.addVisitor(NEW, newv);
    walker.addVisitor(NEW_ARR, nav);

    walker.addVisitor(SET, assignv);

    walker.addVisitor(EQL, eqlv);
    walker.addVisitor(NEQ, neqv);
    walker.addVisitor(GRT, grtv);
    walker.addVisitor(GRE, grev);
    walker.addVisitor(LST, lstv);
    walker.addVisitor(LSE, lsev);

    walker.addVisitor(AND, andv);
    walker.addVisitor(OR, orv);

    walker.addVisitor(INC, incv);
    walker.addVisitor(DEC, decv);
    walker.addVisitor(UNARY_MINUS, negv);

    walker.addVisitor(PRINT, printv);
    walker.addVisitor(READ, readv);

    walker.addVisitor(DEREF, derefv);
    walker.addVisitor(RETURN, retv);

    walker.visit(_ast);
}
