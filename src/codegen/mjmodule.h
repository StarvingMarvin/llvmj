#ifndef MJMODULE_H
#define MJMODULE_H

#include "llvmjConfig.h"

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

#include "parser/parser.h"
#include "symbols/symbols.h"


class MjModule
{
public:
    MjModule(const char* programName);
    MjModule(mj::AST ast);
    void writeAsm(std::ostream out);
    void writeBc(std::ostream out);
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
    
    void astParse(mj::AST ast);
    void programNode(mj::AST ast);
    void defineNode(mj::AST ast);
    void deftypeNode(mj::AST ast);
    void newNode(mj::AST ast);
    void ifNode(mj::AST ast);
    void whileNode(mj::AST ast);
    void breakNode(mj::AST ast);
    void printNode(mj::AST ast);
    void readNode(mj::AST ast);
    void returnNode(mj::AST ast);
    void binopNode(mj::AST ast);
    void unopNode(mj::AST ast);
    void setNode(mj::AST ast);


};

#endif // MJMODULE_H
