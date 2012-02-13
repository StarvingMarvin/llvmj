#include "symbol.h"

Symbol::Symbol(const std::string name, const Type type):
    _name(name),
    _type(type)
{
}

Symbol::Symbol(const Symbol &s):
    _name(s.name()),
    _type(s.type())
{
}

Symbol::Symbol() :
    _type(UNDEFINED_TYPE),
    _name("")
{
}

