#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"

class Symbol
{
private:
    char* _name;
    Type _type;

public:

	Symbol();

    Symbol(char* name, Type type);

    Symbol(const Symbol &s);

    char* name() const { return _name; }

    Type type() const { return _type; }

};

#endif // SYMBOL_H
