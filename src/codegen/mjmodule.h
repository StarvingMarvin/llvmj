
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

    namespace codegen {

    class CodegenVisitor : public NodeVisitor {
        protected:
            CodegenVisitor(llvm::Module &module, const Symbols &symbols);
            llvm::Module &module() const { return _module; }
            const Symbols& symbols() const { return _symbols; }
            static llvm::IRBuilder<> builder;
        private:
            llvm::Module &_module;
            const Symbols &_symbols;
    };

    class VarDesVisitor : public CodegenVisitor{
        public:
            VarDesVisitor(llvm::Module &module, const Symbols &symbols):
                CodegenVisitor(module, symbols){}
            virtual void operator()(AstWalker &walker) const;
    };

    class MethodVisitor : public CodegenVisitor {
        public:
            MethodVisitor(llvm::Module &module, const Symbols &symbols):
                CodegenVisitor(module, symbols){}
            virtual void operator()(AstWalker &walker) const;
    };

    class IntLiteralVisitor : public CodegenVisitor{
        public:
            IntLiteralVisitor(llvm::Module &module, const Symbols &symbols):
                CodegenVisitor(module, symbols){}
            virtual void operator()(AstWalker &walker) const;
    };

    class BinopVisitor : public CodegenVisitor {
        public:
            BinopVisitor(llvm::Module &module, const Symbols &symbols):
                CodegenVisitor(module, symbols){}
            virtual void operator()(AstWalker &walker) const;
            virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const = 0;
    };

    class AddVisitor : public BinopVisitor {
        public:
            AddVisitor(llvm::Module &module, const Symbols &symbols):
                BinopVisitor(module, symbols){}
            virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
    };

    class SubVisitor : public BinopVisitor {
        public:
            SubVisitor(llvm::Module &module, const Symbols &symbols):
                BinopVisitor(module, symbols){}
            virtual llvm::Value* op(llvm::Value* lhs, llvm::Value* rhs) const;
    };

    class NegOpVisitor : public CodegenVisitor{
        public:
            NegOpVisitor(llvm::Module &module, const Symbols &symbols):
                CodegenVisitor(module, symbols){}
            virtual void operator()(AstWalker &walker) const;
    };

    class AssignVisitor : public CodegenVisitor {
        public:
            AssignVisitor(llvm::Module &module, const Symbols &symbols):
                CodegenVisitor(module, symbols){}
            virtual void operator()(AstWalker &walker) const;
    };
    
    }

    typedef std::map<const std::string, llvm::Value*> ValueTable;
    typedef std::map<const std::string, llvm::Type*> TypeTable;
    typedef std::map<const std::string, ValueTable*> LocalValues;

    class Values {
        public:
            Values(llvm::Module* module, const mj::Symbols &symbols);
            llvm::Value* value(const std::string &name) const;
            llvm::Type* type(const std::string &name) const;
            void enterScope(const std::string &name);
            void leaveScope();
        private:
            ValueTable globalValues;
            LocalValues localValues;
            const std::string *currentScope;
            TypeTable types;
    };

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
