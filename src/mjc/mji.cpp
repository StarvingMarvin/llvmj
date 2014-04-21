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
#include "mjc/ezOptionParser.hpp"


using namespace mj;
using namespace std;

void usage(ez::ezOptionParser& opt) {
    std::string usage;
    opt.getUsage(usage);
    std::cout << usage;
}

int main(const int argc, const char** argv) {
    ez::ezOptionParser opt;

    opt.overview = "MicroJava interpreter.";
    opt.syntax = "mji [OPTIONS] FILE";

    opt.add(
        "", // Default.
        0, // Required?
        0, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Display usage instructions.",
        "-h",
        "--help"
    );

    opt.parse(argc, argv);
    std::vector<std::string> badOptions;

    if(!opt.gotExpected(badOptions)) {
        for(unsigned int i=0; i < badOptions.size(); ++i)
            std::cerr << "ERROR: Got unexpected number of arguments for option " << badOptions[i] << ".\n\n";

        usage(opt);
        return 1;
    }

    if (opt.isSet("-h")) {
        usage(opt);
        return 0;
    }

    if (opt.lastArgs.size() != 1) {
        std::cerr << "ERROR: Expected exactly one input file.\n\n";
        usage(opt);
        return 1;
    }


    const string filename = *opt.lastArgs[0];
    try {
        Parser p(filename);
        AST ast = p.parse();
        Symbols symbols;
        checkSemantics(ast, symbols);
        MjModule module(ast, symbols);
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


