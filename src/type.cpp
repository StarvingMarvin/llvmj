#include "type.h"

Type::Type() {
    _name = "MJ.INVALID";
    _valid = false;
}

Type::Type(const char *name)
{
    _name = name;
    _valid = true;
}

Type::Type(const Type &t) {
    _name = t._name;
    _valid = t._valid;
}

