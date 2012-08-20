
#include <cstdlib>
#include <memory>

#include "llvmjConfig.h"
#include "semantics/semantics.h"
#include "parser/parser.h"
#include "codegen/mjmodule.h"

using namespace mj;
using std::auto_ptr;

auto_ptr<Symbols> collectSymbols(AST ast);
MjModule generateCode(AST ast, auto_ptr<Symbols> global);

int main(int argc, char** argv) {

    const char *filename = (argc < 2 || argv[1] == NULL)?
                        "./input" : argv[1];
    Parser p(filename);
    AST ast = p.parse();
    auto_ptr<Symbols> symbols = collectSymbols(ast);
    MjModule module = generateCode(ast, symbols);

    return 0;
}

auto_ptr<Symbols> collectSymbols(AST ast) {
    auto_ptr<Symbols> symbols(new Symbols());


    return symbols;
}

MjModule generateCode(AST ast, auto_ptr<Symbols> symbols) {
    return MjModule("");
}

