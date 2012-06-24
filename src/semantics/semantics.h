
#ifndef _SEMANTICS_H_
#define _SEMANTICS_H_

#include "parser/parser.h"
#include "symbols.h"

namespace mj {

    class BinOpVisitor : public NodeVisitor {
        public:
            BinOpVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
            virtual bool check(Type *l, Type *r) = 0;
        protected:
            Symbols *symbols;
    };

    class SetVisitor : public BinOpVisitor {
        public:
            SetVisitor(Symbols *symbolsTable): BinOpVisitor(symbolsTable){}
            virtual bool check(Type *l, Type *r);
    };

    class VarDesVisitor : public NodeVisitor {
        public:
            VarDesVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class IntLiteralVisitor : public NodeVisitor  {
        public:
            IntLiteralVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class CharLiteralVisitor : public NodeVisitor  {
        public:
            CharLiteralVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class BoolOpVisitor : public BinOpVisitor {
        public:
            BoolOpVisitor(Symbols *symbolsTable): BinOpVisitor(symbolsTable){}
            virtual bool check(Type *l, Type *r);
    };

    class IntOpVisitor : public BinOpVisitor {
        public:
            IntOpVisitor(Symbols *symbolsTable): BinOpVisitor(symbolsTable){}
            virtual bool check(Type *l, Type *r);
    };

    class RelOpVisitor : public BinOpVisitor {
        public:
            RelOpVisitor(Symbols *symbolsTable): BinOpVisitor(symbolsTable){}
            virtual bool check(Type *l, Type *r);
    };

    class DefVisitor : public NodeVisitor {
        public:
            DefVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class MethodVisitor : public NodeVisitor {
        public:
            MethodVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class ClassVisitor : public NodeVisitor {
        public:
            ClassVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class VarVisitor : public NodeVisitor {
        public:
            VarVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class ArrVisitor : public NodeVisitor {
        public:
            ArrVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class PrintVisitor : public NodeVisitor {
        public:
            PrintVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class ReadVisitor : public NodeVisitor {
        public:
            ReadVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class LoopVisitor : public NodeVisitor {
        public:
            LoopVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class CallVisitor : public NodeVisitor {
        public:
            CallVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class FieldDesVisitor : public NodeVisitor {
        public:
            FieldDesVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class ArrDesVisitor : public NodeVisitor {
        public:
            ArrDesVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class NewVisitor : public NodeVisitor {
        public:
            NewVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    Symbols *checkSemantics(AST ast);

}

#endif //_SEMANTICS_H_

