
#include <cstdlib>
#include "scope.h"

using namespace mj;
using std::string;
using std::vector;
using std::map;

Scope::Scope(Scope *parent):
    _parent(parent)
{
}

void Scope::define(Symbol *s) {
    const string name = s->name();
    if (symbolTable[name] != NULL) {
        throw "Symbol already defined!";
    }
    symbolTable[name] = s;
}

const Symbol * Scope::resolve(const string name) {
    const Symbol *res = symbolTable[name];
    if (res == NULL) {
        if (_parent != NULL) {
            res = _parent->resolve(name);
        }
    }
    return res;
}


 
