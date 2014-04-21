
#include "llvmjConfig.h"

#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <map>
#include <stdexcept>

#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/PathV2.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/PassManager.h>

#include "parser/parser.h"
#include "semantics/symbols.h"
#include "semantics/semantics.h"
#include "codegen/values.h"
#include "codegen/mjmodule.h"
#include "codegen/codegen.h"

using namespace mj;
using namespace std;

namespace mj {

class Codegen
{
public:
    Codegen(mj::MjModule &module, CodegenOptions opts):
        _module(module), options(opts){}
    void emitCode() {}
private:

    llvm::TargetMachine *createTargetMachine() {
        return 0;
    }

    llvm::Module &llvmModule() const { return _module.module(); }

    mj::MjModule &_module;
    mj::CodegenOptions options;
    llvm::PassManager *modulePasses;
    llvm::FunctionPassManager *functionPasses;
    llvm::PassManager *codeGenPasses;
};

//PassManager *getPerModulePasses() const {
//    if (!PerModulePasses) {
//        PerModulePasses = new PassManager();
//        PerModulePasses->add(new DataLayoutPass(TheModule));
//        if (TM)
//            TM->addAnalysisPasses(*PerModulePasses);
//    }
//    return PerModulePasses;
//}

//void emitBc(raw_ostream *os) {

//    TargetMachine *tm = CreateTargetMachine(false);
//    getPerModulePasses(TM)->add(createBitcodeWriterPass(*OS));
//}

//void emitLL() {
//    FormattedOS.setStream(*OS, formatted_raw_ostream::PRESERVE_STREAM);
//    getPerModulePasses(TM)->add(createPrintModulePass(&FormattedOS));
//}

//TargetMachine *EmitAssemblyHelper::CreateTargetMachine() {
//// Create the TargetMachine for generating code.
//std::string Error;
//std::string Triple = TheModule->getTargetTriple();
//const llvm::Target *TheTarget = TargetRegistry::lookupTarget(Triple, Error);

//llvm::CodeModel::Model CM = llvm::CodeModel::Default;

//std::string FeaturesStr;

//llvm::Reloc::Model RM = llvm::Reloc::Default;


//CodeGenOpt::Level OptLevel = CodeGenOpt::Default;
//switch (CodeGenOpts.OptimizationLevel) {
//    default: break;
//    case 0: OptLevel = CodeGenOpt::None; break;
//    case 3: OptLevel = CodeGenOpt::Aggressive; break;
//}

//llvm::TargetOptions Options;


//TargetMachine *TM = TheTarget->createTargetMachine(Triple, TargetOpts.CPU,
//                                                   FeaturesStr, Options,
//                                                   RM, CM, OptLevel);


//return TM;
//}

string extension(codegen::Type type) {
    switch (type) {
    case codegen::AST:
        return "ast";
    case codegen::LLVM:
        return "ll";
    case codegen::BC:
        return "bs";
    case codegen::ASM:
        return "s";
    case codegen::OBJ:
        return "obj";
    default:
        throw new runtime_error("Unsupported type");
    }
}

void executeCodegen(CodegenOptions options) {
    llvm::SmallString<128> outpath(options.inputFilePath);
    llvm::sys::path::replace_extension(outpath, extension(options.type));

    Parser p(options.inputFilePath);
    AST ast = p.parse();

    filebuf outfile;
    outfile.open(outpath.c_str(),std::ios::out);
    ostream os(&outfile);

    if (options.type == codegen::AST) {
        pretty_print(ast, os);
        return;
    }

    Symbols symbols;
    checkSemantics(ast, symbols);
    MjModule module(ast, symbols);
    //module.module().dump();
    Codegen cg(module, options);

}

}
