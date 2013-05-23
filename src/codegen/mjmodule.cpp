
#include "llvmjConfig.h"

#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <stdexcept>
#include <vector>

#include <llvm/Analysis/Verifier.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/TypeBuilder.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

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
using llvm::TargetData;
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
    values().define(meth->name(), f);

    BasicBlock *bb = BasicBlock::Create(module().getContext(), "entry", f);
    builder.SetInsertPoint(bb);

    ValueTable local;

    const MethodType &methodType = meth->methodType();
    Scope::iterator sym_it = methodType.arguments().begin();

    for (; sym_it != methodType.arguments().end(); sym_it++) {
        const Symbol &s = *sym_it;
        const NamedValue &nv = dynamic_cast<const NamedValue&>(s);
        llvm::Type *t = values().type(nv.type().name());
        local[nv.name()] = builder.CreateAlloca(t, 0, nv.name());
    }

    Scope &methodScope = meth->scope();
    sym_it = methodScope.begin();

    for (; sym_it != methodScope.end(); sym_it++) {
        const Symbol &s = *sym_it;
        const NamedValue &nv = dynamic_cast<const NamedValue&>(s);
        llvm::Type *t = values().type(nv.type().name());
        local[nv.name()] = builder.CreateAlloca(t, 0, nv.name());
    }

    values().enterScope(local);

    while ( ni != walker.lastChild()) {
        visitChild(walker, ni);
    }

    if (!bb->getTerminator()) {
        builder.CreateRetVoid();
    }

    values().leaveScope();

//    llvm::verifyFunction(*f);

}

void CallVisitor::operator ()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();

    Value *funcVal = visitChild(walker, ni);

    vector<Value*> args;

    while(ni < walker.lastChild()) {
        args.push_back(visitChild(walker, ni));
    }

    string retName = "";

    Function *func = dyn_cast<Function>(funcVal);
    FunctionType *ft = func->getFunctionType();

    if (!ft->getReturnType()->isVoidTy()) {
        retName = "calltmp";
    }

    walker.setData(builder.CreateCall(func, args, retName));

}

void CharLiteralVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * val = tokenText(*b);
    walker.setData(ConstantInt::get(module().getContext(),
                                    APInt(8, val, 10)));
}

void IntLiteralVisitor::operator()(AstWalker &walker) const {
    nodeiterator b = walker.firstChild();
    char * val = tokenText(*b);
    walker.setData(ConstantInt::get(module().getContext(),
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

Value* MulVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateMul(lhs, rhs, "multmp");
}

Value* DivVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateSDiv(lhs, rhs, "divtmp");
}

Value* ModVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateSRem(lhs, rhs, "modtmp");
}

void AssignVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* var = visitChild(walker, ni);
    Value* rhs = visitChild(walker, ni);
    walker.setData(builder.CreateStore(rhs, var));
}

void IncVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* val = visitChild(walker, ni);
    walker.setData(builder.CreateAdd(val,
            ConstantInt::get(module().getContext(),APInt(32, 1, true)),
            "inctmp"));
}

void DecVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* val = visitChild(walker, ni);
    walker.setData(builder.CreateSub(val,
            ConstantInt::get(module().getContext(),APInt(32, 1, true)),
            "dectmp"));
}

void NegOpVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* val = visitChild(walker, ni);
    walker.setData(builder.CreateNeg(val, "negtmp"));
}


void RetVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* val = visitChild(walker, ni);
    walker.setData(builder.CreateRet(val));
}
void FieldDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* name = tokenText(*ni);
    ni++;
    Value *var = values().value(name);
    if (var == NULL) {
        cerr << "ERROR! Unknown variable: " << name << "!" << endl;
        walker.printPosition(cerr)<< "!" << endl;
        return;
    }

    char* fieldName = tokenText(*ni);

    llvm::Type *sppt = var->getType();
    llvm::Type *spt = sppt->getContainedType(0);
    StructType *st = dyn_cast<StructType>(spt->getContainedType(0));

    int idx = values().index(st->getName().str(), fieldName);
    Value *structPtrVal = builder.CreateLoad(var, false, "struct_deref");

    walker.setData(structPtrField(structPtrVal, idx));
}

void ArrDesVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    ni++;
    char* name = tokenText(*ni);
    Value *var = values().value(name);
    Value *index = visitChild(walker, ni);
    Value *val = builder.CreateLoad(var, "array_tmp");
    LLVMContext &ctx = module().getContext();
    vector<Value*> idx;
    idx.push_back(ConstantInt::get(ctx, APInt(32, 0, true)));
    idx.push_back(ConstantInt::get(ctx, APInt(32, 1, true)));
    idx.push_back(index);
    Value *result = builder.CreateGEP(val, idx, "array_field");
    walker.setData(result);
}

void NewVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* typeName = tokenText(*ni);
    llvm::Type *ptype = values().type(typeName);
    llvm::Type *stype = ptype->getContainedType(0);
    StructType *s = dyn_cast<StructType>(stype);
    uint64_t size = sizeOf(s);
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

    uint64_t typeSize = sizeOf(type);
    Value *typeSizeVal = ConstantInt::get(module().getContext(), APInt(32, typeSize, true));

    Value *dataSize = builder.CreateMul(typeSizeVal, arrSize, "data_size_tmp");

    Value *voidPtr = callMalloc(dataSize);
    Value *arrayData = builder.CreateBitCast(voidPtr, ptype, "array_data_ptr");

    uint64_t structSize = sizeOf(atype);
    voidPtr = callMalloc(structSize);
    Value *arrayStruct = builder.CreateBitCast(voidPtr, atype, "array_ptr");


    Value *aSizePtr = structPtrField(arrayStruct, 0);
    builder.CreateStore(arrSize, aSizePtr);

    Value *aDataPtr = structPtrField(arrayStruct, 1);
    builder.CreateStore(arrayData, aDataPtr);

    walker.setData(arrayStruct);

}

void DerefVisitor::operator ()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value *ref = visitChild(walker, ni);
    Value *val = builder.CreateLoad(ref, false, ref->getName());
    walker.setData(val);
}


uint64_t CodegenVisitor::sizeOf(llvm::Type* t) const {
    TargetData td (&_module);
    return td.getTypeAllocSize(t);
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
    Value *idxVal = ConstantInt::get(_module.getContext(), APInt(32, idx, false));
    Value *ptrDeref = ConstantInt::get(_module.getContext(), APInt(32, 0, false));
    vector<Value*> indexes;
    indexes.push_back(ptrDeref);
    indexes.push_back(idxVal);
    return builder.CreateGEP(structPtr, indexes);
}



MjModule::MjModule(AST ast, const Symbols &symbols):
    _ast(ast),
    _symbols(symbols),
    _module(symbols.globalScope().program()->name(), llvm::getGlobalContext()),
    values(&_module, _symbols)
{
    walkTree();
    makeMain();

}

void MjModule::makeMain() {
    FunctionType *main_type = TypeBuilder<llvm::types::i<32>(), true>::get(_module.getContext());
    Function* func_main = Function::Create(
     /*Type=*/main_type,
     /*Linkage=*/GlobalValue::ExternalLinkage,
     /*Name=*/"main", &_module); // (external, no body)
    func_main->setCallingConv(llvm::CallingConv::C);

    BasicBlock *bb = BasicBlock::Create(_module.getContext(), "entry", func_main);
    IRBuilder<> builder(_module.getContext());
    builder.SetInsertPoint(bb);

    Value *mjMain = values.value("main");
    builder.CreateCall(mjMain);
    builder.CreateRet(ConstantInt::get(_module.getContext(), APInt(32, 0)));
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
    NegOpVisitor negv(_module, values);
    IncVisitor incv(_module, values);
    DecVisitor decv(_module, values);
    VarDesVisitor vdv(_module, values);
    DerefVisitor derefv(_module, values);
    RetVisitor retv(_module, values);

    AstWalker walker(defVisitor);

    walker.addVisitor(VAR_DES, vdv);
    walker.addVisitor(FIELD_DES, fdv);
    walker.addVisitor(ARR_DES, adv);
    walker.addVisitor(LIT_INT, ilv);
    walker.addVisitor(LIT_CHAR, clv);
    walker.addVisitor(CALL, callv);

    //walker.addVisitor(WHILE, LoopVisitor(symbolsTable));
    //walker.addVisitor(IF, LoopVisitor(symbolsTable));
    //walker.addVisitor(BREAK, UnexpectedBreakVisitor(symbolsTable));

    walker.addVisitor(PLUS, addv);
    walker.addVisitor(MINUS, subv);
    walker.addVisitor(MUL, mulv);
    walker.addVisitor(DIV, divv);
    walker.addVisitor(MOD, modv);

    walker.addVisitor(DEFFN, methodv);
    walker.addVisitor(NEW, newv);
    walker.addVisitor(NEW_ARR, nav);

    walker.addVisitor(SET, assignv);
//    walker.addVisitor(EQL, ccv);
//    walker.addVisitor(NEQ, ccv);
//    walker.addVisitor(GRT, ccv);
//    walker.addVisitor(GRE, ccv);
//    walker.addVisitor(LST, ccv);
//    walker.addVisitor(LSE, ccv);

    //UnOpVisitor uov(symbolsTable);
    walker.addVisitor(INC, incv);
    walker.addVisitor(DEC, decv);
    walker.addVisitor(UNARY_MINUS, negv);

    //walker.addVisitor(PRINT, PrintVisitor(symbolsTable));
    //walker.addVisitor(READ, ReadVisitor(symbolsTable));

    walker.addVisitor(DEREF, derefv);
    walker.addVisitor(RETURN, retv);

    walker.visit(_ast);
}
