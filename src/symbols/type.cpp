#include "type.h"

//
// Basic types
//

Type::Type(const std::string name):
    _name(name)
{
}

bool Type::operator==(const Type &t) const {
    return _name == t._name;
}

bool Type::compatible(const Type &t) const {
    return *this == t;
}



//
// Reference types
//

ReferenceType::ReferenceType(const std::string name): 
    Type(name)
{}

bool ReferenceType::compatible(const Type &t) const {
    return t == NULL_TYPE || *this == t;
}

//
// Array types
//
ArrayType::ArrayType(const Type &valueType):
    ReferenceType("[" + valueType.name() + "]"),
    _valueType(valueType) {

}

