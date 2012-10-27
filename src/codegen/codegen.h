

#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include "parser/parser.h"
#include "semantics/symbols.h"

namespace mj {
    class AddVisitor : public NodeVisitor {
        public:
            AddVisitor(llvm::Module *module): module(module){}
            virtual void operator()(AstWalker *walker);
        private:
            llvm::Module *module;
    };
}

#endif // _CODEGEN_H_

