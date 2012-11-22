
#ifndef _SEMANTICS_H_
#define _SEMANTICS_H_

#include "parser/parser.h"
#include "symbols.h"

namespace mj {

    class SemanticError {
        public:
            SemanticError(){}

        private:
            int line;
            char* token;
            char* filename;
            char* message;
    };

    class SemanticNodeVisitor : public NodeVisitor {
        public:
            SemanticNodeVisitor(Symbols &symbolsTable): symbols(symbolsTable){}
        protected:
            Symbols &symbols;
    };

    class CheckCompatibleVisitor : public SemanticNodeVisitor {
        public:
            CheckCompatibleVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class VarDesVisitor : public SemanticNodeVisitor {
        public:
            VarDesVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class IntLiteralVisitor : public SemanticNodeVisitor {
        public:
            IntLiteralVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class CharLiteralVisitor : public SemanticNodeVisitor {
        public:
            CharLiteralVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class IntOpVisitor : public SemanticNodeVisitor {
        public:
            IntOpVisitor(Symbols &symbolsTable): SemanticNodeVisitor (symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class UnOpVisitor : public SemanticNodeVisitor {
        public:
            UnOpVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class DefVisitor : public SemanticNodeVisitor {
        public:
            DefVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class MethodVisitor : public SemanticNodeVisitor {
        public:
            MethodVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class ClassVisitor : public SemanticNodeVisitor {
        public:
            ClassVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class VarVisitor : public SemanticNodeVisitor {
        public:
            VarVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class ArrVisitor : public SemanticNodeVisitor {
        public:
            ArrVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class PrintVisitor : public SemanticNodeVisitor {
        public:
            PrintVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class ReadVisitor : public SemanticNodeVisitor {
        public:
            ReadVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class LoopVisitor : public SemanticNodeVisitor {
        public:
            LoopVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class CallVisitor : public SemanticNodeVisitor {
        public:
            CallVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class FieldDesVisitor : public SemanticNodeVisitor {
        public:
            FieldDesVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class ArrDesVisitor : public SemanticNodeVisitor {
        public:
            ArrDesVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class NewVisitor : public SemanticNodeVisitor {
        public:
            NewVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class NewArrVisitor : public SemanticNodeVisitor {
        public:
            NewArrVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    class ProgramVisitor : public SemanticNodeVisitor {
        public:
            ProgramVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker);
    };

    void checkSemantics(AST ast, Symbols &symbols);

}

#endif //_SEMANTICS_H_

