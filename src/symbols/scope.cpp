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

Method::Method(const std::string name, const Type type, Scope *parent):
    Symbol(name, type),
    Scope(name, parent)
{
}

bool Method::matchArguments(std::vector<Type> arguments)
{
    
    int argCount = arguments.size();
    if (argCount != formalArguments.size()) {
        return false;
    }

    for (int i = 0; i < argCount; i++) {
        if (!formalArguments[i].type().compatible(arguments[i])) {
            return false;
        }
    }
    return true;
}

Class::Class(const std::string name, Scope *parent):
    Type(name),
    Symbol(*this),
    Scope(name, parent)
{
}

