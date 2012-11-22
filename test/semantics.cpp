
#include "semantics/semantics.h"
#include <iostream>
#include <cassert>

using namespace mj;

int main (int argc, char** argv) {
    const char *filename = (argc < 2 || argv[1] == NULL)?
                        "./input" : argv[1];

    Parser p(filename);
    AST ast = p.parse();

    Symbols symbols;
    checkSemantics(ast, symbols);
    std::cout << "Symbol table: " << symbols;
    std::cout << std::endl;
}
