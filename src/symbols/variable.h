
#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <string>
#include "type.h"

namespace mj {

    class Variable : public Symbol {
        public:
            Variable(const std::string name, const Type &type);
            const Type &type() const { return _type; }
        private:
            const Type &_type;
    };
}

#endif  //_VARIABLE_H_

