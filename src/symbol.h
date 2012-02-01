#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"

class Symbol
{
private:
    const char* _name;
    Type _type;

public:

	Symbol();

    Symbol(const char* name, Type type);

    Symbol(const Symbol &s);

    const char* name() const { return _name; }

    Type type() const { return _type; }

    bool valid() const { return _type.valid(); }

};

#endif // SYMBOL_H
