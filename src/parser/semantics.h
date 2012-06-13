
#ifndef _SEMANTICS_H_
#define _SEMANTICS_H_

#include "parser.h"
#include "symbols/symbols.h"

namespace mj {

    class BinOpVisitor : NodeVisitor {
        public:
            BinOpVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
            virtual bool check(Type *l, Type *r) = 0;
        protected:
            Symbols *symbols;
    };

    class SetVisitor : BinOpVisitor {
        public:
            SetVisitor(Symbols *symbolsTable): BinOpVisitor(symbolsTable){}
            virtual bool check(Type *l, Type *r);
    };

    class GetVisitor : NodeVisitor {
        public:
            GetVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class IntLiteralVisitor : NodeVisitor  {
        public:
            IntLiteralVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class CharLiteralVisitor : NodeVisitor  {
        public:
            CharLiteralVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class BoolOpVisitor : BinOpVisitor {
        public:
            BoolOpVisitor(Symbols *symbolsTable): BinOpVisitor(symbolsTable){}
            virtual bool check(Type *l, Type *r);
    };

    class IntOpVisitor : BinOpVisitor {
        public:
            IntOpVisitor(Symbols *symbolsTable): BinOpVisitor(symbolsTable){}
            virtual bool check(Type *l, Type *r);
    };

    class RelOpVisitor : BinOpVisitor {
        public:
            RelOpVisitor(Symbols *symbolsTable): BinOpVisitor(symbolsTable){}
            virtual bool check(Type *l, Type *r);
    };

    class DefVisitor : NodeVisitor {
        public:
            DefVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class FuncVisitor : NodeVisitor {
        public:
            FuncVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class ClassVisitor : NodeVisitor {
        public:
            ClassVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class VarVisitor : NodeVisitor {
        public:
            VarVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class PrintVisitor : NodeVisitor {
        public:
            PrintVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class ReadVisitor : NodeVisitor {
        public:
            ReadVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class LoopVisitor : NodeVisitor {
        public:
            LoopVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class CallVisitor : NodeVisitor {
        public:
            CallVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class DotVisitor : NodeVisitor {
        public:
            DotVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class IndexVisitor : NodeVisitor {
        public:
            IndexVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class NewVisitor : NodeVisitor {
        public:
            NewVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    Symbols *checkSemantics(AST ast);

}

#endif //_SEMANTICS_H_

