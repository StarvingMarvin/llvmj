
#include <cstdlib>

#include "llvmjConfig.h"
#include "semantics/semantics.h"
#include "parser/parser.h"
#include "codegen/mjmodule.h"

using namespace mj;

MjModule generateCode(AST ast, Symbols &global);

int main(int argc, char** argv) {

    const char *filename = (argc < 2 || argv[1] == NULL)?
                        "./input" : argv[1];
    Parser p(filename);
    AST ast = p.parse();
    Symbols symbols;
    checkSemantics(ast, symbols);
    MjModule module = generateCode(ast, symbols);

    return 0;
}

MjModule generateCode(AST ast, Symbols &symbols) {
    return MjModule("");
}

