#include "llvmjConfig.h"

#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <stdexcept>
#include <vector>

#include <llvm/Analysis/Verifier.h>
#include <llvm/Constants.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Function.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Instructions.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/TypeBuilder.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Type.h>
#include <llvm/Value.h>

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
    initPrimitives();
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

void Values::initPrimitives() {
    LLVMContext &ctx = _module->getContext();
    types["int"] = IntegerType::getInt32Ty(ctx);
    types["char"] = IntegerType::getInt8Ty(ctx);
    types["void"] = llvm::Type::getVoidTy(ctx);
    types["mj.null"] = PointerType::get(IntegerType::getInt8Ty(ctx), 0);
}

void Values::initTypes(const GlobalScope &global) {
    const Program *program = global.program();

    SplitScope &ps = dynamic_cast<SplitScope&>(program->scope());
    array_type_iterator arr_it = global.arrayTypesBegin();
    class_iterator class_it = ps.classBegin();
    llvm::Type *arrayType = NULL;

    while (class_it != ps.classEnd()) {

        while (arr_it != global.arrayTypesEnd()
               && (arrayType = initArrayType(**arr_it)) != NULL) {
            types[(*arr_it)->name()] = arrayType;
            arr_it++;
        }

        const Class *c = *class_it;
        llvm::Type * classType = initClass(*c);


        if (classType != NULL) {
            types[c->name()] = classType;
            class_it++;
        }

    }

    while (arr_it != global.arrayTypesEnd()) {
        types[(*arr_it)->name()] = initArrayType(**arr_it);
        arr_it++;
    }
}

llvm::Type *Values::initClass(const mj::Class &c) {

    string moduleId = _module->getModuleIdentifier();
    string structName = moduleId + "::" + c.name();

    StructType *st = _module->getTypeByName(structName);

    if (st == NULL) {
        st = StructType::create(_module->getContext(), structName);
    }
    PointerType *spt = PointerType::get(st, 0);


    //types[c->name()] = spt;
    Scope::iterator csit = c.classScope().begin();
    vector<llvm::Type*> classBody;
    int idx = 0;
    for (; csit != c.classScope().end(); csit++) {
        const Symbol &fieldSymbol = *csit;
        const NamedValue &classField = dynamic_cast<const NamedValue&>(fieldSymbol);
        const string fieldTypeName = classField.type().name();

        llvm::Type *fieldType = (fieldTypeName == c.name())?
            spt :
            types[fieldTypeName];


        if (fieldType == NULL) {
            return NULL;
        }
        classBody.push_back(fieldType);
        llvm::Twine fullName = st->getName() + "." + classField.name();
        fieldIndices[fullName.str()] = idx;
        idx++;
    }
    st->setBody(classBody);
    return spt;
}

llvm::Type *Values::initArrayType(const mj::ArrayType &at) {
    llvm::Type *type = types[at.valueType().name()];
    if (type == NULL) {
        return NULL;
    }
    llvm::Type *ptype = PointerType::get(type, 0);
    LLVMContext &ctx = _module->getContext();
    vector<llvm::Type*> fields;
    fields.push_back(IntegerType::get(ctx, 64));
    fields.push_back(ptype);
    return StructType::get(ctx, fields);
}

void Values::initGlobals(const GlobalScope &global) {
    constant_iterator const_it = global.constantBegin();
    for (; const_it != global.constantEnd(); const_it++) {
        const Constant *c = *const_it;
        llvm::Type *t = types[c->type().name()];
        GlobalVariable *gv = new GlobalVariable(*_module, t, true,
                                     GlobalValue::ExternalLinkage, 0, c->name());
        gv->setInitializer(ConstantInt::get(t, c->value(), true));
        globalValues[c->name()] = gv;
    }

    const Program *p = global.program();
    const_it = p->programScope().constantBegin();
    for (; const_it != p->programScope().constantEnd(); const_it++) {
        const Constant *c = *const_it;
        llvm::Type *t = types[c->type().name()];

        string name = p->name() + "::" + c->name();
        GlobalVariable *gv = new GlobalVariable(*_module, t, true,
                                     GlobalValue::ExternalLinkage, 0, name);

        gv->setInitializer(ConstantInt::get(t, c->value(), true));
        globalValues[c->name()] = gv;
    }

    variable_iterator var_it = p->programScope().variableBegin();
    for (; var_it != p->programScope().variableEnd(); var_it++) {
        const NamedValue *v = *var_it;
        llvm::Type *t = types[v->type().name()];

        string name = p->name() + "::" + v->name();
        GlobalVariable *gv = new GlobalVariable(*_module, t, false,
                                     GlobalValue::ExternalLinkage, 0, name);

        gv->setInitializer(ConstantInt::get(t, 0, true));
        globalValues[v->name()] = gv;
    }

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
