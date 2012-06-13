
#ifndef _SEMANTICS_H_
#define _SEMANTICS_H_

#include "parser.h"
#include "symbols/symbols.h"

namespace mj {

    class SetVisitor : NodeVisitor {
        public:
            SetVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
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

    class BoolOpVisitor : NodeVisitor {
        public:
            BoolOpVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class IntOpVisitor : NodeVisitor {
        public:
            IntOpVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class RelOpVisitor : NodeVisitor {
        public:
            RelOpVisitor(Symbols *symbolsTable): symbols(symbolsTable){}
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
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

