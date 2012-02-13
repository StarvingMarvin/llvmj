#include "scope.h"

Scope::Scope(const std::string name, Scope *parent):
    _name(name),
    _parent(parent)
{
    //symbolTable = llvm::StringMap<Symbol>();
}

void Scope::define(const Symbol *s) {
    symbolTable[s->name()] = s;
    //Symbol s1 = s;
}

const Symbol * Scope::resolve(const std::string name) {
    return symbolTable[name];
    //return Symbol();
}

