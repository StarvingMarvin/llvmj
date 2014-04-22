

#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include <string>

namespace mj {


namespace codegen {
    enum Type {AST, LLVM, BC, ASM, OBJ};
}

class CodegenOptions
{
public:
    codegen::Type type;
    int optLevel;
    std::string inputFilePath;
};


void executeCodegen(CodegenOptions options);

}

#endif // _CODEGEN_H_

