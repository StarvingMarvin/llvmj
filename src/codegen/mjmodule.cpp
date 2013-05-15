
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
#include "semantics/semantics.h"
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

    Scope &methodScope = meth->scope();

    BasicBlock *bb = BasicBlock::Create(module().getContext(), "entry", f);
    builder.SetInsertPoint(bb);
    Scope::iterator sym_it = methodScope.begin();
    ValueTable local;
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

    builder.CreateRetVoid();

    values().leaveScope();

    llvm::verifyFunction(*f);

}

void CallVisitor::operator ()(AstWalker &walker) const {

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
            ConstantInt::get(module().getContext(),APInt(32, 1, 10)),
            "inctmp"));
}

void DecVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* val = visitChild(walker, ni);
    walker.setData(builder.CreateSub(val,
            ConstantInt::get(module().getContext(),APInt(32, 1, 10)),
            "dectmp"));
}

void NegOpVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* val = visitChild(walker, ni);
    walker.setData(builder.CreateNeg(val, "negtmp"));
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
    char* name = tokenText(*ni);
    values().value(name);

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
    char* sizeStr = tokenText(*ni);
    int arrSize;
    istringstream(sizeStr) >> arrSize;

    llvm::Type *type = values().type(typeName);
    llvm::Type *ptype = PointerType::get(type, 0);
    llvm::Type *atype = arrayType(ptype);

    uint64_t typeSize = sizeOf(type);
    Value *voidPtr = callMalloc(typeSize * arrSize);
    Value *arrayData = builder.CreateBitCast(voidPtr, ptype, "array_data_ptr");

    uint64_t structSize = sizeOf(atype);
    voidPtr = callMalloc(structSize);
    Value *arrayStruct = builder.CreateBitCast(voidPtr, atype, "array_ptr");


    Value *aSizePtr = structPtrField(arrayStruct, 0);
    builder.CreateStore(ConstantInt::get(module().getContext(), APInt(64, arrSize, 10)), aSizePtr);

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

Values::Values(llvm::Module *module, const Symbols &symbols):
    _module(module)
{

    const GlobalScope &global = symbols.globalScope();
    initPrimitives(global);
    initStructs(global);
    initGlobals(global);
    initMethods(global);

    FunctionType *malloc_type = TypeBuilder<llvm::types::i<8>*(llvm::types::i<64>), true>::get(module->getContext());
    Function* func_malloc = Function::Create(
     /*Type=*/malloc_type,
     /*Linkage=*/GlobalValue::ExternalLinkage,
     /*Name=*/"malloc", module); // (external, no body)
    func_malloc->setCallingConv(llvm::CallingConv::C);

}

void Values::initPrimitives(const GlobalScope &global) {
    LLVMContext &ctx = _module->getContext();
    types["int"] = IntegerType::getInt32Ty(ctx);
    types["char"] = IntegerType::getInt8Ty(ctx);
    types["void"] = llvm::Type::getVoidTy(ctx);
}

void Values::initStructs(const GlobalScope &global) {
    const Program *program = global.program();

    SplitScope &ps = dynamic_cast<SplitScope&>(program->scope());
    class_iterator class_it = ps.classBegin();
    for (; class_it != ps.classEnd(); class_it++) {

        const Class *c = *class_it;

        cout << *c << endl;

        StructType *st = StructType::create(_module->getContext(), program->name() + "::" + c->name());
        PointerType *spt = PointerType::get(st, 0);

        types[c->name()] = spt;
        Scope::iterator csit = c->scope().begin();
        vector<llvm::Type*> classBody;
        int idx = 0;
        for (; csit != c->scope().end(); csit++) {
            const Symbol &fieldSymbol = *csit;
            const NamedValue &classField = dynamic_cast<const NamedValue&>(fieldSymbol);
            classBody.push_back(types[classField.type().name()]);
            llvm::Twine fullName = st->getName() + "." + classField.name();
            fieldIndices[fullName.str()] = idx;
            idx++;
        }
        st->setBody(classBody);
    }
}

void Values::initGlobals(const GlobalScope &global) {

}

void Values::initMethods(const GlobalScope &global) {
    method_type_iterator prototype_it = global.methodPrototypesBegin();
    for (; prototype_it != global.methodPrototypesEnd(); prototype_it++) {
        const MethodType * prototype = *prototype_it;
        vector<llvm::Type*> args;
        MethodArguments &arguments = prototype->arguments();
        ArgumentTypes::const_iterator arg_it = arguments.begin();
        for (; arg_it != arguments.end(); arg_it++) {
            args.push_back(types[(*arg_it)->name()]);
        }
        FunctionType* ft = FunctionType::get(types[prototype->returnType().name()], args, false);
        types[prototype->name()] = ft;
    }
}



llvm::Value* Values::value(const string &name) const {

    Value* ret = NULL;

    if (localScope != NULL) {
        ValueTable::const_iterator local_it = localScope->find(name);
        if (local_it != localScope->end()) {
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

int Values::index(const std::string &structName, const std::string &fieldName) const {
    string qualified = structName + "." + fieldName;
    IndexTable::const_iterator it = fieldIndices.find(qualified);
    if (it != fieldIndices.end()) {
        return it->second;
    }
    return -1;
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
    vector<Value*> args;
    args.push_back(ConstantInt::get(_module.getContext(), APInt(64, size, 10)));
    Value* vMalloc = _module.getFunction("malloc");
    return builder.CreateCall(vMalloc, args, "voidptr");
}

llvm::Value* CodegenVisitor::structPtrField(llvm::Value *structPtr, int idx) const {
    Value *idxVal = ConstantInt::get(_module.getContext(), APInt(32, idx, true));
    Value *ptrDeref = ConstantInt::get(_module.getContext(), APInt(32, 0, true));
    vector<Value*> indexes;
    indexes.push_back(ptrDeref);
    indexes.push_back(idxVal);
    return builder.CreateGEP(structPtr, indexes);
}

void Values::enterScope(ValueTable &local) {
    localScope = &local;
}

void Values::leaveScope() {
    localScope = NULL;
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

    AddVisitor addv(_module, values);
    AssignVisitor assignv(_module, values);
    FieldDesVisitor fdv(_module, values);
    ArrDesVisitor adv(_module, values);
    IntLiteralVisitor ilv(_module, values);
    CharLiteralVisitor clv(_module, values);
    MethodVisitor methodv(_module, values, _symbols);
    CallVisitor callv(_module, values, _symbols);
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

    walker.visit(_ast);
}
