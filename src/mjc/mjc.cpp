
#include "llvmjConfig.h"

#include <string>

#include "codegen/codegen.h"
#include "mjc/ezOptionParser.hpp"


using namespace mj;
using namespace std;

void usage(ez::ezOptionParser& opt) {
    string usage;
    opt.getUsage(usage);
    cout << usage;
}

int main(const int argc, const char** argv) {
    ez::ezOptionParser opt;

    opt.overview = "MicroJava compiler.";
    opt.syntax = "mjc [OPTIONS] FILE";
    opt.example = "mjc -t llvm program.mj\n";

    opt.add(
        "", // Default.
        0, // Required?
        0, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Display usage instructions.",
        "-h",
        "--help"
    );

    opt.add(
        "obj", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Output type. Posible values are: ast, llvm, bc, asm and obj. Default is obj",
        "-t",
        "--output-type"
    );

    opt.add(
        "2", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Optimization level. Possible values: 0-3",
        "-O"
    );

    opt.parse(argc, argv);
    vector<string> badOptions;

    if(!opt.gotExpected(badOptions)) {
        for(unsigned int i=0; i < badOptions.size(); ++i)
            cerr << "ERROR: Got unexpected number of arguments for option " << badOptions[i] << ".\n\n";

        usage(opt);
        return 1;
    }

    if (opt.isSet("-h")) {
        usage(opt);
        return 0;
    }

    if (opt.lastArgs.size() != 1) {
        cerr << "ERROR: Expected exactly one input file.\n\n";
        usage(opt);
        return 1;
    }

    string output_type;
    opt.get("-t")->getString(output_type);

    int optLevel;
    opt.get("-O")->getInt(optLevel);

    CodegenOptions cgOptions;
    cgOptions.inputFilePath = *opt.lastArgs[0];

    if (output_type == "ast") {
        cgOptions.type = codegen::AST;
    } else if (output_type == "llvm") {
        cgOptions.type = codegen::LLVM;
    } else if (output_type == "bc") {
        cgOptions.type = codegen::BC;
    } else if (output_type == "asm") {
        cgOptions.type = codegen::ASM;
    } else if (output_type == "obj") {
        cgOptions.type = codegen::OBJ;
    } else {
        cerr << "ERROR: Unknown output type: '" << output_type << "'" << endl;
        usage(opt);
        return 1;
    }

    cgOptions.optLevel = optLevel;

    try {
        executeCodegen(cgOptions);
    } catch (exception& e) {
        cerr << "Error occured: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown error occured!" << endl;
        return 1;
    }

    return 0;
}


