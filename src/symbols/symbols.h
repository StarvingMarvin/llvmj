
#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#include "symbol.h"
#include "type.h"
#include "variable.h"
#include "method.h"
#include "class.h"
#include "scope.h"

#include <memory>

namespace mj {

    class Symbols {
        public:
            Symbols(){}

        private:
            Scope *currentScope;
    };

    std::auto_ptr<Scope> makeGlobalScope();

}

#endif //_SYMBOLS_H_
