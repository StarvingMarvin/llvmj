#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"

class Symbol
{
private:
    char* _name;
    Type _type;

public:

    Symbol(char* name, Type type);

    char* name() { return _name; }

    Type type() { return _type; }

};

#endif // SYMBOL_H
