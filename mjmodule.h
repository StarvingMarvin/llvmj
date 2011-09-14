#ifndef MJMODULE_H
#define MJMODULE_H

#include "llvmjConfig.h"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

using namespace llvm;

class MjModule
{
public:
    MjModule(const char* programName);
private:
    Module *module;

    Type *MJChar;
    Type *MJInt;
    Type *MJArray;

    Function *mainWrap;
    Function *ord;
    Function *chr;
    Function *len;

};

#endif // MJMODULE_H
