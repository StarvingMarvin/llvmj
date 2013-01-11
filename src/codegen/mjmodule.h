
#ifndef MJMODULE_H
#define MJMODULE_H


namespace mj {

    namespace codegen {

    class CodegenVisitor : public NodeVisitor {
        protected:
            CodegenVisitor(llvm::Module *module, Symbols &symbols);
            llvm::Module *module() const { return _module; }
            Symbols& symbols() const { return _symbols; }
            static llvm::IRBuilder<> builder;
        private:
            llvm::Module *_module;
            Symbols &_symbols;
    };

    class VarDesVisitor : public CodegenVisitor{
        public:
            VarDesVisitor(llvm::Module *module, Symbols &symbols): 
                CodegenVisitor(module, symbols){}
            virtual void operator()(AstWalker &walker) const;
    };

    class IntLiteralVisitor : public CodegenVisitor{
        public:
            IntLiteralVisitor(llvm::Module *module, Symbols &symbols): 
                CodegenVisitor(module, symbols){}
            virtual void operator()(AstWalker &walker) const;
    };

    class AddVisitor : public CodegenVisitor{
        public:
            AddVisitor(llvm::Module *module, Symbols &symbols): 
                CodegenVisitor(module, symbols){}
            virtual void operator()(AstWalker &walker) const;
    };

    class SubVisitor : public CodegenVisitor{
        public:
            SubVisitor(llvm::Module *module, Symbols &symbols): 
                CodegenVisitor(module, symbols){}
            virtual void operator()(AstWalker &walker) const;
    };

    class NegOpVisitor : public CodegenVisitor{
        public:
            NegOpVisitor(llvm::Module *module, Symbols &symbols): 
                CodegenVisitor(module, symbols){}
            virtual void operator()(AstWalker &walker) const;
    };

    class AssignVisitor : public CodegenVisitor {
        public:
            AssignVisitor(llvm::Module *module, Symbols &symbols): 
                CodegenVisitor(module, symbols){}
            virtual void operator()(AstWalker &walker) const;
    };
    
    }

    typedef std::map<const std::string, llvm::Value*> ValueTable;

    class Values {
        public:
            Values(Values* parent): _parent(parent) {}
            void define(const std::string &name, llvm::Value* v);
            llvm::Value* resolve(const std::string &name);
            ~Values();
        private:
            Values *_parent;
            ValueTable values;
    };

    class MjModule
    {
        public:
            MjModule(AST ast, Symbols &s);
            llvm::Module &module() { return _module; }
        private:
            AST _ast;
            Symbols &symbols;
            llvm::Module _module;

            Values values;

            void init();

    };

    //MjModule

}

#endif // MJMODULE_H
