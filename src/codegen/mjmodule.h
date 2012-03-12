#ifndef MJMODULE_H
#define MJMODULE_H

#include "llvmjConfig.h"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>


class MjModule
{
public:
    MjModule(const char* programName);
private:
    llvm::Module *module;

    llvm::Type *MJChar;
    llvm::Type *MJInt;
    llvm::Type *MJArray;

    llvm::Function *mainWrap;
    llvm::Function *ord;
    llvm::Function *chr;
    llvm::Function *len;

};

#endif // MJMODULE_H
