
#include "llvmjConfig.h"

#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <map>
#include <stdexcept>

#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/PathV2.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/TargetRegistry.h>
#include "llvm/Target/TargetLibraryInfo.h"
#include <llvm/Target/TargetMachine.h>
#include "llvm/Transforms/IPO.h"
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
        _module(module), options(opts), modulePasses(0), functionPasses(0),
        codegenPasses(0), _targetMachine(0) {}
    void emitCode(llvm::raw_ostream *os);

private:

    llvm::TargetMachine *targetMachine();
    void createPasses();

    llvm::Module &llvmModule() const { return _module.module(); }
    bool nativeEnabled() const { return options.type == codegen::ASM || options.type == codegen::OBJ; }

    mj::MjModule &_module;
    mj::CodegenOptions options;
    llvm::PassManager *modulePasses;
    llvm::FunctionPassManager *functionPasses;
    llvm::PassManager *codegenPasses;
    llvm::TargetMachine *_targetMachine;
};


llvm::TargetMachine *Codegen::targetMachine() {

    if (!_targetMachine) {

        //// Create the TargetMachine for generating code.
        llvm::InitializeNativeTarget();
        string error;
        string triple = llvmModule().getTargetTriple();
        const llvm::Target *target = llvm::TargetRegistry::lookupTarget(triple, error);

        llvm::CodeModel::Model cm = llvm::CodeModel::Default;

        string features;

        llvm::Reloc::Model rm = llvm::Reloc::Default;

        llvm::CodeGenOpt::Level optLevel = llvm::CodeGenOpt::Default;
        switch (options.optLevel) {
            case 0: optLevel = llvm::CodeGenOpt::None; break;
            case 3: optLevel = llvm::CodeGenOpt::Aggressive; break;
            default: break;
        }

        llvm::TargetOptions targetOptions;

        _targetMachine = target->createTargetMachine(triple, "",
                                                           features, targetOptions,
                                                           rm, cm, optLevel);
    }
    return _targetMachine;
}

//bool EmitAssemblyHelper::AddEmitPasses(BackendAction Action,
//                                       formatted_raw_ostream &OS,
//                                       TargetMachine *TM) {

//  // Normal mode, emit a .s or .o file by running the code generator. Note,
//  // this also adds codegenerator level optimization passes.
//  TargetMachine::CodeGenFileType CGFT = TargetMachine::CGFT_AssemblyFile;
//  if (Action == Backend_EmitObj)
//    CGFT = TargetMachine::CGFT_ObjectFile;
//  else if (Action == Backend_EmitMCNull)
//    CGFT = TargetMachine::CGFT_Null;
//  else
//    assert(Action == Backend_EmitAssembly && "Invalid action!");


//  if (TM->addPassesToEmitFile(*PM, OS, CGFT,
//                              /*DisableVerify=*/!CodeGenOpts.VerifyModule)) {
//    Diags.Report(diag::err_fe_unable_to_interface_with_target);
//    return false;
//  }

//  return true;
//}

void Codegen::createPasses() {

    llvm::TargetMachine *tm = targetMachine();
    llvm::DataLayout *dl = new llvm::DataLayout(&llvmModule());

    modulePasses = new llvm::PassManager();
    modulePasses->add(dl);
    functionPasses = new llvm::FunctionPassManager(&llvmModule());
    functionPasses->add(dl);

    tm->addAnalysisPasses(*modulePasses);
    tm->addAnalysisPasses(*functionPasses);

    llvm::PassManagerBuilder pmBuilder;
    pmBuilder.OptLevel = options.optLevel;

    // Figure out TargetLibraryInfo.
    llvm::Triple targetTriple(llvmModule().getTargetTriple());
    pmBuilder.LibraryInfo = new llvm::TargetLibraryInfo(targetTriple);

    if (nativeEnabled()) {
        codegenPasses = new llvm::PassManager();
        codegenPasses->add(dl);
        codegenPasses->add(pmBuilder.LibraryInfo);
        tm->addAnalysisPasses(*codegenPasses);
    }

    unsigned inlineThreshold = 225;
    if (options.optLevel > 2) {
        inlineThreshold = 275;
        modulePasses->add(llvm::createStripSymbolsPass(true));
    }
    pmBuilder.Inliner = llvm::createFunctionInliningPass(inlineThreshold);
    pmBuilder.populateFunctionPassManager(*functionPasses);
    pmBuilder.populateModulePassManager(*modulePasses);
}

void Codegen::emitCode(llvm::raw_ostream *os) {
    llvm::formatted_raw_ostream formattedOS;

    createPasses();

    llvm::TargetMachine::CodeGenFileType cgft = llvm::TargetMachine::CGFT_AssemblyFile;
    switch (options.type) {
    case codegen::LLVM:
        formattedOS.setStream(*os, llvm::formatted_raw_ostream::PRESERVE_STREAM);
        modulePasses->add(llvm::createPrintModulePass(&formattedOS));
        break;
    case codegen::BC:
        modulePasses->add(llvm::createBitcodeWriterPass(*os));
        break;
    case codegen::ASM:
        break;
    case codegen::OBJ:
        cgft = llvm::TargetMachine::CGFT_ObjectFile;
        break;
    default:
        // screem
        break;
    }

    functionPasses->doInitialization();
    for (llvm::Module::iterator I = llvmModule().begin(),
         E = llvmModule().end(); I != E; ++I)
        if (!I->isDeclaration())
            functionPasses->run(*I);
    functionPasses->doFinalization();

    modulePasses->run(llvmModule());

    if (codegenPasses) {
        formattedOS.setStream(*os, llvm::formatted_raw_ostream::PRESERVE_STREAM);
        targetMachine()->addPassesToEmitFile(*codegenPasses, formattedOS, cgft);
        codegenPasses->run(llvmModule());
    }
}

string extension(codegen::Type type) {
    switch (type) {
    case codegen::AST:
        return "ast";
    case codegen::LLVM:
        return "ll";
    case codegen::BC:
        return "bc";
    case codegen::ASM:
        return "s";
    case codegen::OBJ:
        return "o";
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

    llvm::raw_os_ostream raw_os(os);

    Symbols symbols;
    checkSemantics(ast, symbols);
    MjModule module(ast, symbols);
    Codegen cg(module, options);
    cg.emitCode(&raw_os);

}

}
