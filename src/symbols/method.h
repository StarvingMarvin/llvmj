
#ifndef _METHOD_H_
#define _METHOD_H_

#include "type.h"
#include "variable.h"
#include "scope.h"

namespace mj {
    
    class MethodScope: public Scope {
        public:
            MethodScope(Scope *parent);
            virtual void define(Symbol *s);
            virtual Symbol *resolve(const std::string name);
    };

    class Method : public Symbol {
        friend class MethodScope;
        public:
            Method(const std::string name, 
                    const Type returnType);
        private:
            Type _returnType;
            std::vector<Variable*> _arguments;
            MethodScope *methodScope;
    };

}

#endif //_METHOD_H_

