#ifndef TYPE_H
#define TYPE_H

#include <iostream>
#include "symbol.h"

namespace mj {

    class Type : public Symbol
    {
        public:
            Type(const std::string name);
            virtual bool operator==(const Type &t) const;
            virtual bool compatible(const Type &t) const;
    };

    const Type NULL_TYPE("null");
    const Type VOID_TYPE("void");
    const Type UNDEFINED_TYPE("MJ.undefined");

    class ReferenceType: public Type {
        public:
            ReferenceType(const std::string name);
            bool compatible(const Type &t) const;
    };

    class ArrayType: public ReferenceType {
        private:
            const Type &_valueType;
        public:
            ArrayType(const Type &valueType);
            const Type & valuetType() const { return _valueType; }
    };

}

#endif // TYPE_H
