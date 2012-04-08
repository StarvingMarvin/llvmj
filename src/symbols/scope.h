#ifndef SCOPE_H
#define SCOPE_H

#include "symbol.h"

#include <memory>
#include <vector>
#include <map>
#include <cstdlib>

namespace mj {

    class Scope
    {
        public:
            Scope(Scope *parent);

            Scope *parent() const { return _parent; }

            virtual void define (Symbol *s);

            virtual const Symbol * resolve(const std::string name);

        protected:
            std::map<const std::string, const Symbol*> symbolTable;

        private:
            Scope(Scope & s){}
            void operator=(Scope& s){}
            Scope *_parent;

    };

}

#endif // SCOPE_H

