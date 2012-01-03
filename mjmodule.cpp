#include "mjmodule.h"

MjModule::MjModule(const char *programName)
{
    module = new Module(programName, getGlobalContext());

}
