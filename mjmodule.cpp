#include "mjmodule.h"

MjModule::MjModule()
{
    module = new Module("MjModule", getGlobalContext());

}
