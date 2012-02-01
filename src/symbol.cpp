#include "symbol.h"

Symbol::Symbol(const char* name, Type type)
{
    this->_name = name;
    this->_type = type;
}

Symbol::Symbol(const Symbol &s) {
	_name = s._name;
    _type = s._type;
}

Symbol::Symbol() {
	_name = "";
}

