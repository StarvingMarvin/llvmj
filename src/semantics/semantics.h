
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
            virtual void operator()(AstWalker &walker) const;
    };

    class VarDesVisitor : public SemanticNodeVisitor {
        public:
            VarDesVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class IntLiteralVisitor : public SemanticNodeVisitor {
        public:
            IntLiteralVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class CharLiteralVisitor : public SemanticNodeVisitor {
        public:
            CharLiteralVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class IntOpVisitor : public SemanticNodeVisitor {
        public:
            IntOpVisitor(Symbols &symbolsTable): SemanticNodeVisitor (symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class UnOpVisitor : public SemanticNodeVisitor {
        public:
            UnOpVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class MethodVisitor : public SemanticNodeVisitor {
        public:
            MethodVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class ClassVisitor : public SemanticNodeVisitor {
        public:
            ClassVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class VarVisitor : public SemanticNodeVisitor {
        public:
            VarVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class ArrVisitor : public SemanticNodeVisitor {
        public:
            ArrVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class PrintVisitor : public SemanticNodeVisitor {
        public:
            PrintVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class ReadVisitor : public SemanticNodeVisitor {
        public:
            ReadVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class LoopVisitor : public SemanticNodeVisitor {
        public:
            LoopVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class CallVisitor : public SemanticNodeVisitor {
        public:
            CallVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class FieldDesVisitor : public SemanticNodeVisitor {
        public:
            FieldDesVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class ArrDesVisitor : public SemanticNodeVisitor {
        public:
            ArrDesVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class NewVisitor : public SemanticNodeVisitor {
        public:
            NewVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class NewArrVisitor : public SemanticNodeVisitor {
        public:
            NewArrVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    class ProgramVisitor : public SemanticNodeVisitor {
        public:
            ProgramVisitor(Symbols &symbolsTable): SemanticNodeVisitor(symbolsTable){}
            virtual void operator()(AstWalker &walker) const;
    };

    void checkSemantics(AST ast, Symbols &symbols);

}

#endif //_SEMANTICS_H_

