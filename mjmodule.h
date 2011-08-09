#ifndef MJMODULE_H
#define MJMODULE_H

#include "llvmjConfig.h"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

using namespace llvm;

class MjModule
{
public:
    MjModule();
private:
    Module *module;
    Type *MJChar;
    Type *MJInt;
};

#endif // MJMODULE_H
