
#include "llvmjConfig.h"

#include <iostream>
#include <map>
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
    //Value* result = builder.CreateLoad(values().value(ident), ident);
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

    //llvm::verifyFunction(*f);

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
    if (isa<PointerType>(lhs->getType())) {
        lhs = builder.CreateLoad(lhs, false, lhs->getName());
    }

    Value* rhs = visitChild(walker, ni);
    if (isa<PointerType>(rhs->getType())) {
        rhs = builder.CreateLoad(rhs, false, rhs->getName());
    }
    Value* result = op(lhs, rhs);
    walker.setData(result);
}

Value* AddVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateAdd(lhs, rhs, "addtmp");
}

Value* SubVisitor::op(Value* lhs, Value* rhs) const {
    return builder.CreateSub(lhs, rhs, "subtmp");
}

void AssignVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    Value* var = visitChild(walker, ni);
    Value* rhs = visitChild(walker, ni);
    walker.setData(builder.CreateStore(rhs, var));
}

void NegOpVisitor::operator()(AstWalker &walker) const {
    
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

    int idx = values().index(name, fieldName);
    Value *idxVal = ConstantInt::get(module().getContext(), APInt(32, idx, true));
    Value *structVal = builder.CreateLoad(var, false, "struct_deref");
    walker.setData(builder.CreateGEP(structVal, idxVal));
}

void NewVisitor::operator()(AstWalker &walker) const {
    nodeiterator ni = walker.firstChild();
    char* typeName = tokenText(*ni);
    llvm::Type *ptype = values().type(typeName);
    llvm::Type *stype = ptype->getContainedType(0);
    StructType *s = dyn_cast<StructType>(stype);
    TargetData td (&module());
    uint64_t size = td.getTypeAllocSize(s);
    vector<Value*> args;
    args.push_back(ConstantInt::get(module().getContext(), APInt(64, size, 10)));
    Value* vMalloc = module().getFunction("malloc");
    Value* voidPtr = builder.CreateCall(vMalloc, args, s->getName() + "_voidptr");
    Value* structPtr = builder.CreateBitCast(voidPtr, ptype, s->getName() + "_ptr");
    walker.setData(structPtr);
}

Values::Values(llvm::Module *module, const Symbols &symbols) {
    const GlobalScope &global = symbols.globalScope();
    LLVMContext &ctx = module->getContext();

    //    type_iterator type_it = global.typesBegin();
    //    type_iterator type_end = global.typesEnd();
    //    for(; type_it != type_end; type_it++) {
    //        const mj::Type *t = *type_it;

    //    }

    types["int"] = IntegerType::getInt32Ty(ctx);
    types["char"] = IntegerType::getInt8Ty(ctx);
    types["void"] = llvm::Type::getVoidTy(ctx);

    //array_t

    method_type_iterator prototype_it = global.methodPrototypesBegin();
    for (; prototype_it != global.methodPrototypesEnd(); prototype_it++) {
        const MethodType * prototype = *prototype_it;
        vector<llvm::Type*> *args = new vector<llvm::Type*>();
        MethodArguments &arguments = prototype->arguments();
        ArgumentTypes::const_iterator arg_it = arguments.begin();
        for (; arg_it != arguments.end(); arg_it++) {
            args->push_back(types[(*arg_it)->name()]);
        }
        FunctionType* ft = FunctionType::get(types[prototype->returnType().name()], *args, false);
        types[prototype->name()] = ft;
    }

    FunctionType *malloc_type = TypeBuilder<llvm::types::i<8>*(llvm::types::i<64>), true>::get(ctx);
    Function* func_malloc = Function::Create(
     /*Type=*/malloc_type,
     /*Linkage=*/GlobalValue::ExternalLinkage,
     /*Name=*/"malloc", module); // (external, no body)
    func_malloc->setCallingConv(llvm::CallingConv::C);

    //    constant_iterator const_it = global.constantBegin();
    //    constant_iterator const_end = global.constantEnd();
    //    for(;const_it != const_end; const_it++) {
    //        const Constant *c = *const_it;
    //    }

    const Program *program = global.program();

    SplitScope &ps = dynamic_cast<SplitScope&>(program->scope());
    class_iterator class_it = ps.classBegin();
    for (; class_it != ps.classEnd(); class_it++) {

        const Class *c = *class_it;

        cout << *c << endl;

        StructType *st = StructType::create(ctx, program->name() + "::" + c->name());
        PointerType *spt = PointerType::get(st, 0);

        types[c->name()] = spt;
        Scope::iterator csit = c->scope().begin();
        vector<llvm::Type*> *classBody = new vector<llvm::Type*>();
        int idx = 0;
        for (; csit != c->scope().end(); csit++) {
            const Symbol &fieldSymbol = *csit;
            const NamedValue &classField = dynamic_cast<const NamedValue&>(fieldSymbol);
            classBody->push_back(types[classField.type().name()]);
            fieldIndices[c->name()+"."+classField.name()] = idx;
            idx++;
        }
        st->setBody(*classBody);
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
    IntLiteralVisitor ilv(_module, values);
    MethodVisitor methodv(_module, values, _symbols);
    NewVisitor newv(_module, values);
    SubVisitor subv(_module, values);
    VarDesVisitor vdv(_module, values);


    AstWalker walker(defVisitor);

    walker.addVisitor(VAR_DES, vdv);
    walker.addVisitor(FIELD_DES, fdv);
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
    walker.addVisitor(NEW, newv);
    //walker.addVisitor(NEW_ARR, NewArrVisitor(symbolsTable));

    //CheckCompatibleVisitor ccv(symbolsTable);
    walker.addVisitor(SET, assignv);
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
