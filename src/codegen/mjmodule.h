
#ifndef MJMODULE_H
#define MJMODULE_H

// Depends on:
//  
// <string>
// <map>
//
// <llvm/DerivedTypes.h>
// <llvm/Support/IRBuilder.h>
// <llvm/Module.h>
//
// "parser/parser.h"
// "semantics/semantics.h"
//

namespace mj {

typedef std::map<const std::string, llvm::Value*> ValueTable;
typedef std::map<const std::string, llvm::Type*> TypeTable;

class Values {
public:
    Values(llvm::Module* module, const mj::Symbols &symbols);
    llvm::Value* value(const std::string &name) const;
    llvm::Type* type(const std::string &name) const;
    void enterScope(ValueTable &local);
    void leaveScope();
private:
    ValueTable globalValues;
    ValueTable *localScope;
    TypeTable types;
};

namespace codegen {

class CodegenVisitor : public NodeVisitor {
protected:
    CodegenVisitor(llvm::Module &module, Values &values);
    llvm::Module &module() const { return _module; }
    Values &values() const { return _values; }
    static llvm::IRBuilder<> builder;
private:
    llvm::Module &_module;
    Values &_values;
};

class VarDesVisitor : public CodegenVisitor{
public:
    VarDesVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class MethodVisitor : public CodegenVisitor {
public:
    MethodVisitor(llvm::Module &module, Values &values, const Symbols &symbols):
        CodegenVisitor(module, values), symbols(symbols){}
    virtual void operator()(AstWalker &walker) const;
private:
    const Symbols &symbols;
};

class IntLiteralVisitor : public CodegenVisitor{
public:
    IntLiteralVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class BinopVisitor : public CodegenVisitor {
public:
    BinopVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const = 0;
};

class AddVisitor : public BinopVisitor {
public:
    AddVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class SubVisitor : public BinopVisitor {
public:
    SubVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class AssignVisitor : public BinopVisitor {
public:
    AssignVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class NegOpVisitor : public CodegenVisitor{
public:
    NegOpVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

}


class MjModule
{
public:
    MjModule(AST ast, const mj::Symbols &symbols);
    llvm::Module &module() { return _module; }
private:
    AST _ast;
    const Symbols &_symbols;
    llvm::Module _module;
    Values values;
    void walkTree();
};


}

#endif // MJMODULE_H
