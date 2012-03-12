#include "mjmodule.h"

using namespace llvm;

MjModule::MjModule(const char *programName)
{
    module = new Module(programName, getGlobalContext());

}
