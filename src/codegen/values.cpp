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
#include "semantics/symbols.h"
#include "semantics/semantics.h"
#include "codegen/values.h"

using namespace std;
using namespace mj;

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


Values::Values(llvm::Module *module, const Symbols &symbols):
    _module(module)
{

    const GlobalScope &global = symbols.globalScope();
    initPrimitives(global);
    initTypes(global);
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

void Values::initTypes(const GlobalScope &global) {
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

llvm::Type *initClass(const mj::Class &c) {
    return NULL;
}

llvm::Type *initArrayType(const mj::ArrayType &at) {
    return NULL;
}

void Values::initGlobals(const GlobalScope &global) {
//    IRBuilder<> builder(_module->getContext());
    //    constant_iterator const_it = global.constantBegin();
//    for (; const_it < global.constantEnd(); const_it++) {
//        GlobalVariable::get()
//    }
}

void Values::initMethods(const GlobalScope &global) {
    method_type_iterator prototype_it = global.methodPrototypesBegin();
    for (; prototype_it != global.methodPrototypesEnd(); prototype_it++) {
        const MethodType * prototype = *prototype_it;
        vector<llvm::Type*> args;
        MethodArguments &arguments = prototype->arguments();
        ArgumentTypes::const_iterator arg_it = arguments.typesBegin();
        for (; arg_it != arguments.typesEnd(); arg_it++) {
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

    if (ret == NULL) {
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

void Values::define(std::string name, llvm::Value *value) {
    globalValues[name] = value;
}

void Values::enterScope(ValueTable &local) {
    localScope = &local;
}

void Values::leaveScope() {
    localScope = NULL;
}
