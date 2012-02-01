#ifndef TYPE_H
#define TYPE_H

#include "llvmjConfig.h"

class Type
{
private:
    char *_name;
    bool _valid;
public:
    Type();
    Type(char *name);
    Type(const Type &t);
    char *name() const { return _name; }
    bool valid() const { return _valid; }
};

#endif // TYPE_H
