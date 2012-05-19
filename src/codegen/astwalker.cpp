
#include "codegen/astwalker.h"

using mj::AST;
using mj::tokenType;
using mj::tokenText;
using llvm::Module;
using llvm::getGlobalContext;

InvalidAst::InvalidAst(string message):
    _message(message)
{
}

llvm::Module Program::operator(AST ast) {
    if (tokenType(ast) != PROGRAM) {
        throw InvalidAst("Expecting `PROGRAM` token.");
    }
    string name = tokenText(ast);
    Module *module = new Module(name, getGlobalContext());
    return module;
}
