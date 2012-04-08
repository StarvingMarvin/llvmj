
#include "class.h"

using namespace mj;
using std::string;
using std::vector;

Class::Class(string name):
    Type(name)
{
}

ClassScope::ClassScope(Scope *parent):
    Scope(parent)
{
}

void ClassScope::define(Symbol *s) {
    Scope::define(s);
}

Symbol *ClassScope::resolve(const string name) {
    // don't look in parent scopes
    return _symbolTable[name];
}

