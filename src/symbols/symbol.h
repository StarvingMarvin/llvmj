#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <iostream>

namespace mj {

    class Symbol
    {
        private:
            std::string _name;

        public:

            Symbol(const std::string name);

            const std::string name() const { return _name; }

    };

}

std::ostream& operator<<(std::ostream &os, const mj::Symbol& s);

#endif // SYMBOL_H
