#include "symbol.h"

using namespace MJ;

Symbol::Symbol(const std::string name, const Type type):
    _name(name),
    _type(type)
{
}

Symbol::Symbol() :
    _type(UNDEFINED_TYPE),
    _name("")
{
}

VariableSymbol::VariableSymbol(const std::string name, const Type type, const bool final):
    Symbol(name, type),
    _final(final)
{
}

