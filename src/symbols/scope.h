#ifndef SCOPE_H
#define SCOPE_H

#include "symbol.h"
#include "llvm/ADT/StringMap.h"
#include <vector>

namespace MJ {

    class Scope
    {
        private:
            const std::string _name;
            Scope *_parent;
            llvm::StringMap<const Symbol*> symbolTable;
            Scope(Scope & s){}
            void operator=(Scope& s){}

        public:
            Scope(const std::string name, Scope *parent);

            const std::string name() const { return _name; }

            Scope *parent() const { return _parent; }

            void define (const Symbol *s);

            const Symbol * resolve(const std::string name);

    };

    class Method : public Symbol, public Scope {
        public:
            Method(const std::string name, const Type type, Scope *parent);
            bool matchArguments(std::vector<Type> args);
        private:
            std::vector<Symbol> formalArguments;
    };

    class Class : public Type, public Symbol, public Scope {
        public:
            Class(std::string name, Scope *parent);
    };

}

#endif // SCOPE_H

