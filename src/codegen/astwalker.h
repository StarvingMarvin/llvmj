
#ifndef _ASTWALKER_H_
#define _AST_WALKER_H_

#include "parser/parser.h"
#include "llvm/Module.h"

namespace mj {

    class InvalidAst {
    public:
        InvalidAst(string message);
        string message();
    private:
        const string _message;
    };

    class Program : public AstFunctor<llvm::Module*> {
    public:
        virtual llvm::Module* operator(AST ast);
    };
}

#endif // _ASTWALKER_H_

