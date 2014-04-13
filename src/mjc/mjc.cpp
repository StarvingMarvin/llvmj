
#include "llvmjConfig.h"

#include <string>
#include <iostream>
#include <stack>
#include <vector>
#include <map>

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "llvmjConfig.h"
#include "parser/parser.h"
#include "semantics/symbols.h"
#include "semantics/semantics.h"
#include "codegen/values.h"
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
        //module.module().dump();
        module.run(argc, argv);
    } catch (exception& e) {
        cerr << "Error occured: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown error occured!" << endl;
        return 1;
    }

    return 0;
}


