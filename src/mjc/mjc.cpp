
#include <string>
#include <iostream>
#include <stack>
#include <vector>
#include <map>

#include "llvmjConfig.h"
#include "semantics/semantics.h"
#include "parser/parser.h"
#include "codegen/mjmodule.h"


using namespace mj;
using namespace std;

MjModule generateCode(AST ast, Symbols &global);

int main(int argc, char** argv) {

    const char *filename = (argc < 2 || argv[1] == NULL)?
                        "./input" : argv[1];
    try {
    Parser p(filename);
    AST ast = p.parse();
    Symbols symbols;
    checkSemantics(ast, symbols);
    MjModule module = generateCode(ast, symbols);
    } catch (exception& e) {
        cerr << "Error occured: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown error occured!" << endl;
        return 1;
    }

    return 0;
}

MjModule generateCode(AST ast, Symbols &symbols) {
    return MjModule("");
}

