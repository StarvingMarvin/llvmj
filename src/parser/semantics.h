
#ifndef _SEMANTICS_H_
#define _SEMANTICS_H_

#include "parser.h"
#include "symbols/symbols.h"

namespace mj {

    class AssignVisitor : NodeVisitor {
        public:
            AssignVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class BoolOpVisitor : NodeVisitor {
        public:
            BoolOpVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class IntOpVisitor : NodeVisitor {
        public:
            IntOpVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class DefVisitor : NodeVisitor {
        public:
            DefVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class FuncVisitor : NodeVisitor {
        public:
            FuncVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class ClassVisitor : NodeVisitor {
        public:
            ClassVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class VarVisitor : NodeVisitor {
        public:
            VarVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class PrintVisitor : NodeVisitor {
        public:
            PrintVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class ReadVisitor : NodeVisitor {
        public:
            ReadVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class LoopVisitor : NodeVisitor {
        public:
            LoopVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class CallVisitor : NodeVisitor {
        public:
            CallVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class DotVisitor : NodeVisitor {
        public:
            DotVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class IndexVisitor : NodeVisitor {
        public:
            IndexVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    class NewVisitor : NodeVisitor {
        public:
            NewVisitor(Symbols *symbolsTable);
            virtual void operator()(AstWalker *walker);
        private:
            Symbols *symbols;
    };

    Symbols *checkSemantics(AST ast);

}

#endif //_SEMANTICS_H_

