
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
    void emitCode(llvm::raw_ostream *os);
private:

    llvm::TargetMachine *createTargetMachine();
    void createPasses(llvm::TargetMachine *tm);

    llvm::Module &llvmModule() const { return _module.module(); }

    mj::MjModule &_module;
    mj::CodegenOptions options;
    llvm::PassManager *modulePasses;
    llvm::FunctionPassManager *functionPasses;
    llvm::PassManager *codeGenPasses;
};


llvm::TargetMachine *Codegen::createTargetMachine() {
    //// Create the TargetMachine for generating code.
    string error;
    string triple = llvmModule().getTargetTriple();
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(triple, error);

    llvm::CodeModel::Model cm = llvm::CodeModel::Default;

    string features;

    llvm::Reloc::Model rm = llvm::Reloc::Default;

    llvm::CodeGenOpt::Level optLevel = llvm::CodeGenOpt::Default;
    switch (options.optLevel) {
        case 0: optLevel = CodeGenOpt::None; break;
        case 3: optLevel = CodeGenOpt::Aggressive; break;
        default: break;
    }

    llvm::TargetOptions targetOptions;


    llvm::TargetMachine *tm = target->createTargetMachine(triple, "",
                                                       features, targetOptions,
                                                       rm, cm, optLevel);


    return tm;
}

void Codegen::createPasses(llvm::TargetMachine *tm) {
  llvm::PassManagerBuilder pmBuilder;
  pmBuilder.OptLevel = options.optLevel;

  // Figure out TargetLibraryInfo.
  llvm::Triple targetTriple(llvmModule().getTargetTriple());
  pmBuilder.LibraryInfo = new llvm::TargetLibraryInfo(targetTriple);
  if (!CodeGenOpts.SimplifyLibCalls)
    PMBuilder.LibraryInfo->disableAllFunctions();

  switch (Inlining) {
  case CodeGenOptions::NoInlining: break;
  case CodeGenOptions::NormalInlining: {
    // FIXME: Derive these constants in a principled fashion.
    unsigned Threshold = 225;
    if (CodeGenOpts.OptimizeSize == 1)      // -Os
      Threshold = 75;
    else if (CodeGenOpts.OptimizeSize == 2) // -Oz
      Threshold = 25;
    else if (OptLevel > 2)
      Threshold = 275;
    PMBuilder.Inliner = createFunctionInliningPass(Threshold);
    break;
  }
  case CodeGenOptions::OnlyAlwaysInlining:
    // Respect always_inline.
    if (OptLevel == 0)
      // Do not insert lifetime intrinsics at -O0.
      PMBuilder.Inliner = createAlwaysInlinerPass(false);
    else
      PMBuilder.Inliner = createAlwaysInlinerPass();
    break;
  }

  // Set up the per-function pass manager.
  FunctionPassManager *FPM = getPerFunctionPasses(TM);
  if (CodeGenOpts.VerifyModule)
    FPM->add(createVerifierPass());
  PMBuilder.populateFunctionPassManager(*FPM);

  // Set up the per-module pass manager.
  PassManager *MPM = getPerModulePasses(TM);

  if (!CodeGenOpts.DisableGCov &&
      (CodeGenOpts.EmitGcovArcs || CodeGenOpts.EmitGcovNotes)) {
    // Not using 'GCOVOptions::getDefault' allows us to avoid exiting if
    // LLVM's -default-gcov-version flag is set to something invalid.
    GCOVOptions Options;
    Options.EmitNotes = CodeGenOpts.EmitGcovNotes;
    Options.EmitData = CodeGenOpts.EmitGcovArcs;
    memcpy(Options.Version, CodeGenOpts.CoverageVersion, 4);
    Options.UseCfgChecksum = CodeGenOpts.CoverageExtraChecksum;
    Options.NoRedZone = CodeGenOpts.DisableRedZone;
    Options.FunctionNamesInData =
        !CodeGenOpts.CoverageNoFunctionNamesInData;
    MPM->add(createGCOVProfilerPass(Options));
    if (CodeGenOpts.getDebugInfo() == CodeGenOptions::NoDebugInfo)
      MPM->add(createStripSymbolsPass(true));
  }

  PMBuilder.populateModulePassManager(*MPM);
}

void Codegen::emitCode(llvm::raw_ostream *os) {
  llvm::formatted_raw_ostream formattedOS;

  TargetMachine *tm = createTargetMachine();
  CreatePasses(tm);

  switch (Action) {
  case Backend_EmitNothing:
    break;

  case Backend_EmitBC:
    getPerModulePasses(TM)->add(createBitcodeWriterPass(*OS));
    break;

  case Backend_EmitLL:
    FormattedOS.setStream(*OS, formatted_raw_ostream::PRESERVE_STREAM);
    getPerModulePasses(TM)->add(createPrintModulePass(&FormattedOS));
    break;

  default:
    FormattedOS.setStream(*OS, formatted_raw_ostream::PRESERVE_STREAM);
    if (!AddEmitPasses(Action, FormattedOS, TM))
      return;
  }

  // Before executing passes, print the final values of the LLVM options.
  cl::PrintOptionValues();

  // Run passes. For now we do all passes at once, but eventually we
  // would like to have the option of streaming code generation.

  if (PerFunctionPasses) {
    PrettyStackTraceString CrashInfo("Per-function optimization");

    PerFunctionPasses->doInitialization();
    for (Module::iterator I = TheModule->begin(),
           E = TheModule->end(); I != E; ++I)
      if (!I->isDeclaration())
        PerFunctionPasses->run(*I);
    PerFunctionPasses->doFinalization();
  }

  if (PerModulePasses) {
    PrettyStackTraceString CrashInfo("Per-module optimization passes");
    PerModulePasses->run(*TheModule);
  }

  if (CodeGenPasses) {
    PrettyStackTraceString CrashInfo("Code generation");
    CodeGenPasses->run(*TheModule);
  }
}

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
