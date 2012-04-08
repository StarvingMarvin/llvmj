
#include "variable.h"

using namespace mj;
using std::string;

Variable::Variable(const string name, const Type &type):
    Symbol(name),
    _type(type)
{
}

