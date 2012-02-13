#ifndef SCOPE_H
#define SCOPE_H

#include "symbol.h"
#include "llvm/ADT/StringMap.h"


class Scope
{
private:
    const std::string _name;
    Scope *_parent;
    llvm::StringMap<const Symbol*> symbolTable;

public:
    Scope(const std::string name, Scope *parent);

    const std::string name() const { return _name; }

    Scope *parent() const { return _parent; }

    void define (const Symbol *s);

    const Symbol * resolve(const std::string name);

};

#endif // SCOPE_H
