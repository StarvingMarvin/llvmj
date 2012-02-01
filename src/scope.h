#ifndef SCOPE_H
#define SCOPE_H

#include "symbol.h"
#include "llvm/ADT/StringMap.h"


class Scope
{
private:
    char *_name;
    Scope *_parent;
    llvm::StringMap<Symbol> symbolTable;

public:
    Scope(char *name, Scope *parent);

    char *name() { return _name; }

    Scope *parent() { return _parent; }

    void define (const Symbol &s);

    Symbol resolve(char *name);

};

#endif // SCOPE_H
