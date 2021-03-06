
#ifndef MJMODULE_H
#define MJMODULE_H

// Depends on:
//  
// <string>
// <map>
// <memory>
//
// <llvm/DerivedTypes.h>
// <llvm/Support/IRBuilder.h>
// <llvm/Module.h>
//
// "parser/parser.h"
// "semantics/semantics.h"
// "codegen/values.h"
//

namespace mj {

namespace codegen {

class CodegenVisitor : public NodeVisitor {
protected:
    CodegenVisitor(llvm::Module &module, Values &values);
    llvm::Module &module() const { return _module; }
    Values &values() const { return _values; }
    static llvm::IRBuilder<> builder;
    size_t sizeOf(llvm::Type* t) const;
    llvm::Type* arrayType(llvm::Type *t) const;
    llvm::Value* callMalloc(size_t size) const;
    llvm::Value* callMalloc(llvm::Value* size) const;
    llvm::Value* structPtrField(llvm::Value *structPtr, int idx) const;
    llvm::Value* cast(llvm::Value* val, llvm::Type* type) const;
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

class FieldDesVisitor : public CodegenVisitor{
public:
    FieldDesVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class ArrDesVisitor : public CodegenVisitor{
public:
    ArrDesVisitor(llvm::Module &module, Values &values):
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

class CallVisitor : public CodegenVisitor {
public:
    CallVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values) {}
    virtual void operator()(AstWalker &walker) const;
};

class NewVisitor : public CodegenVisitor {
public:
    NewVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values) {}
    virtual void operator()(AstWalker &walker) const;
};

class NewArrVisitor : public CodegenVisitor {
public:
    NewArrVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values) {}
    virtual void operator()(AstWalker &walker) const;
};

class IntLiteralVisitor : public CodegenVisitor{
public:
    IntLiteralVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class CharLiteralVisitor : public CodegenVisitor{
public:
    CharLiteralVisitor(llvm::Module &module, Values &values):
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

class MulVisitor : public BinopVisitor {
public:
    MulVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class DivVisitor : public BinopVisitor {
public:
    DivVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class ModVisitor : public BinopVisitor {
public:
    ModVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class EqlVisitor : public BinopVisitor {
public:
    EqlVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class NeqVisitor : public BinopVisitor {
public:
    NeqVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class GrtVisitor : public BinopVisitor {
public:
    GrtVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class GreVisitor : public BinopVisitor {
public:
    GreVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class LstVisitor : public BinopVisitor {
public:
    LstVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class LseVisitor : public BinopVisitor {
public:
    LseVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class AndVisitor : public BinopVisitor {
public:
    AndVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class OrVisitor : public BinopVisitor {
public:
    OrVisitor(llvm::Module &module, Values &values):
        BinopVisitor(module, values){}
    virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
};

class AssignVisitor : public CodegenVisitor {
public:
    AssignVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class IncVisitor : public CodegenVisitor{
public:
    IncVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class DecVisitor : public CodegenVisitor{
public:
    DecVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class NegOpVisitor : public CodegenVisitor{
public:
    NegOpVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class WhileVisitor : public CodegenVisitor{
public:
    WhileVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class IfVisitor : public CodegenVisitor{
public:
    IfVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class BreakVisitor : public CodegenVisitor{
public:
    BreakVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class RetVisitor : public CodegenVisitor{
public:
    RetVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class DerefVisitor : public CodegenVisitor{
public:
    DerefVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class PrintVisitor : public CodegenVisitor{
public:
    PrintVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

class ReadVisitor : public CodegenVisitor{
public:
    ReadVisitor(llvm::Module &module, Values &values):
        CodegenVisitor(module, values){}
    virtual void operator()(AstWalker &walker) const;
};

}

class MjModule
{
public:
    MjModule(AST ast, const mj::Symbols &symbols);
    llvm::Module &module() { return *_module; }
    void run(int argc, const char **argv);
private:
    AST _ast;
    const Symbols &_symbols;
    std::unique_ptr<llvm::Module> _module;
    Values values;

    void walkTree();
    void makeStdLib();
    void makeMain();
};


}

#endif // MJMODULE_H
