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

//void MjModule::astParse(AST ast) {
//    
//    uint32_t type = tokenType(ast);
//
//    switch(type) {
//    case PROGRAM:
//        break;
//    case DEF:
//        break;
//    case CLASS:
//    case FN:
//    case VAR:
//    case ARR:
//    case CONST:
//        //def
//        break;
//    case NEW:
//        break;
//    case IF:
//        break;
//    case WHILE:
//        break;
//    case BREAK:
//        break;
//    case PRINT:
//        break;
//    case READ:
//        break;
//    case RETURN:
//        break;
//    case PLUS:
//    case MINUS:
//    case MUL:
//    case DIV:
//    case MOD:
//    case OR:
//    case AND:
//    case EQL:
//    case NEQ:
//    case GRT:
//    case GRE:
//    case LST:
//    case LSE:
//        //operations
//        break;
//    case INC:
//    case DEC:
//        break;
//    case SET:
//        break;
//    }
//
//}

