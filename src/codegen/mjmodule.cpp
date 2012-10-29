#include "mjmodule.h"

using namespace llvm;
using mj::AST;
using mj::tokenType;

MjModule::MjModule(AST ast)
{
    //module = new Module(programName, getGlobalContext());

}



MjModule::MjModule(const char* name) {

}

void MjModule::writeAsm(std::ostream &out) {
  verifyModule(*module, PrintMessageAction);
  PassManager PM;
  raw_os_ostream ros(out);
  PM.add(createPrintModulePass(&ros));
  PM.run(*module);
}

void MjModule::writeBc(std::ostream &out) {
  verifyModule(*module, PrintMessageAction);
  PassManager PM;
  raw_os_ostream ros(out);
  PM.add(createPrintModulePass(&ros));
  PM.run(*module);
}

MjModule::~MjModule() {
}


