#include "symbol.h"

Symbol::Symbol(char* name, Type type)
{
    this->_name = name;
    this->_type = type;
}

Symbol::Symbol(const Symbol &s) {
	
}

Symbol::Symbol() {
	
}
