#include "symbol.h"

using namespace mj;
using std::string;
using std::ostream;

Symbol::Symbol(const string name):
    _name(name)
{
}

ostream& operator<<(ostream &os, const Symbol& s) {
    return os << s.name();
}

