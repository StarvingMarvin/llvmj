#ifndef MJMODULE_H
#define MJMODULE_H

#include "llvmjConfig.h"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>

#include "parser/parser.h"
#include "symbols/symbols.h"


class MjModule
{
public:
    MjModule(const char* programName);
    MjModule(mj::AST ast);
    void operator=(MjModule &other);
    ~MjModule();
    void writeAsm(std::ostream &out);
    void writeBc(std::ostream &out);
private:
    llvm::Module *module;

    llvm::Type *MJChar;
    llvm::Type *MJInt;
    llvm::Type *MJArray;

    llvm::Function *mainWrap;
    llvm::Function *ord;
    llvm::Function *chr;
    llvm::Function *len;

    mj::Symbols symbolTable;
    

    void init();


};

#endif // MJMODULE_H
