
#ifndef _CLASS_H_
#define _CLASS_H_

#include "type.h"
#include "scope.h"

namespace mj {

    class ClassScope : public Scope {
        public:
            ClassScope(Scope *parent);
            virtual void define(Symbol *s);
            virtual Symbol *resolve(const std::string name);
    };

    class Class : public Type {
        friend class ClassScope;
        public:
            Class(std::string name);
        private:
            std::vector<Variable> _fields;
            ClassScope *classScope;
    };

}

#endif //_CLAS_H_

