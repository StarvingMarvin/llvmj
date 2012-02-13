#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"

class Symbol
{
private:
    const std::string _name;
    const Type _type;

public:

	Symbol();

    Symbol(const std::string name, const Type type);

    Symbol(const Symbol &s);

    const std::string name() const { return _name; }

    const Type type() const { return _type; }

    bool defined() const { return ! (_type == UNDEFINED_TYPE); }

};

#endif // SYMBOL_H
