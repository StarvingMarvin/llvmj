
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

    class MjModule
    {
        public:
            MjModule(const char* programName);
            MjModule(mj::AST ast);
            void operator=(MjModule &other);
            ~MjModule();
            void writeAsm(std::ostream &out);
            void writeBc(std::ostream &out);
        private:
            llvm::Module *module;

            llvm::Type *MJChar;
            llvm::Type *MJInt;
            llvm::Type *MJArray;

            llvm::Function *mainWrap;
            llvm::Function *ord;
            llvm::Function *chr;
            llvm::Function *len;

            //Symbols &symbols;


            void init();


    };

    //MjModule

}

#endif // MJMODULE_H
