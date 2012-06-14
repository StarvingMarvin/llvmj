
#include <cstdlib>
#include <memory>

#include "llvmjConfig.h"
#include "semantics/semantics.h"
#include "parser/parser.h"
#include "codegen/mjmodule.h"

using namespace mj;
using std::auto_ptr;

auto_ptr<Scope> collectSymbols(AST ast);
MjModule generateCode(AST ast, auto_ptr<Scope> global);

int main(int argc, char** argv) {

    const char *filename = (argc < 2 || argv[1] == NULL)?
                        "./input" : argv[1];
    Parser p(filename);
    AST ast = p.parse();
    auto_ptr<Scope> global = collectSymbols(ast);
    MjModule module = generateCode(ast, global);

    return 0;
}

auto_ptr<Scope> collectSymbols(AST ast) {
    auto_ptr<Scope> global = makeGlobalScope();



    return global;
}

MjModule generateCode(AST ast, auto_ptr<Scope> global) {
    return MjModule("");
}

