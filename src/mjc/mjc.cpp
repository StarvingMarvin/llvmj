
#include "llvmjConfig.h"

#include <string>
#include <iostream>
#include <stack>
#include <vector>
#include <map>

#include <llvm/DerivedTypes.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

#include "llvmjConfig.h"
#include "semantics/semantics.h"
#include "parser/parser.h"
#include "codegen/mjmodule.h"


using namespace mj;
using namespace std;

int main(int argc, char** argv) {

    const char *filename = (argc < 2 || argv[1] == NULL)?
                        "./input" : argv[1];
    try {
        Parser p(filename);
        AST ast = p.parse();
        Symbols symbols;
        checkSemantics(ast, symbols);
        MjModule module(ast, symbols);
    } catch (exception& e) {
        cerr << "Error occured: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown error occured!" << endl;
        return 1;
    }

    return 0;
}


