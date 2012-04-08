
#include "method.h"

using namespace mj;
using std::string;
using std::vector;

Method::Method(const string name, 
        const Type returnType):
    Symbol(name),
    _returnType(returnType)
{
}

MethodScope::MethodScope (Scope *parent):
    Scope(parent)
{
}

void MethodScope::define(Symbol *s) {
}

Symbol *MethodScope::resolve(const std::string name) {
}


