#include "scope.h"

Scope::Scope(char *name, Scope *parent)
{
    _name = name;
    _parent = parent;
    //symbolTable = llvm::StringMap<Symbol>();
}

void Scope::define(const Symbol &s) {
    symbolTable[s.name()] = s;
}

Symbol Scope::resolve(char *name) {
    return symbolTable[name];
}
