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

void MjModule::astParse(AST ast) {
    
    uint32_t type = tokenType(ast);

    switch(type) {
    case PROGRAM:
        break;
    case DEF:
        break;
    case CLASS:
    case FN:
    case VAR:
    case ARR:
    case CONST:
        //def
        break;
    case NEW:
        break;
    case IF:
        break;
    case WHILE:
        break;
    case BREAK:
        break;
    case PRINT:
        break;
    case READ:
        break;
    case RETURN:
        break;
    case PLUS:
    case MINUS:
    case MUL:
    case DIV:
    case MOD:
    case OR:
    case AND:
    case EQL:
    case NEQ:
    case GRT:
    case GRE:
    case LST:
    case LSE:
        //operations
        break;
    case INC:
    case DEC:
        break;
    case SET:
        break;
    }

}

void MjModule::programNode(mj::AST ast) {

}

void MjModule::defineNode(mj::AST ast) {

}

void MjModule::deftypeNode(AST ast) {

}

void MjModule::newNode(AST ast) {

}

void MjModule::ifNode(AST ast) {

}

void MjModule::whileNode(AST ast) {

}

void MjModule::breakNode(AST ast) {

}

void MjModule::printNode(AST ast) {

}

void MjModule::readNode(AST ast) {

}

void MjModule::returnNode(AST ast) {

}

void MjModule::binopNode(AST ast) {

}

void MjModule::unopNode(AST ast) {

}

void MjModule::setNode(AST ast) {

}

