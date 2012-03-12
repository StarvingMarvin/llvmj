
#include <cstdlib>
#include "symbols/scope.h"
#include "parser/parser.h"
#include "codegen/mjmodule.h"

using namespace MJ;

Scope *collectSymbols(AST ast);
MjModule generateCode(AST ast, Scope *global);

int main(int argc, char** argv) {

    const char *filename = (argc < 2 || argv[1] == NULL)?
                        "./input" : argv[1];
    AST ast = parse(filename);
    Scope *global = collectSymbols(ast);
    MjModule module = generateCode(ast, global);

    return 0;
}

Scope *collectSymbols(AST ast) {
    return new Scope("", NULL);
}

MjModule generateCode(AST ast, Scope *global) {
    return MjModule("");
}

