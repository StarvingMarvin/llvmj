
#ifndef PARSER_H
#define PARSER_H

#include "parser/MicroJavaLexer.h"
#include "parser/MicroJavaParser.h"
#include <string>

namespace mj {

    typedef pANTLR3_BASE_TREE AST;

    class ParserException {
        public:
            ParserException(const std::string message): _message(message){}
            const std::string message() const { return _message; }
        private:
            const std::string _message;
    };

    class Parser {
        public:
            Parser(const std::string filename);
            AST parse() throw(ParserException);
            ~Parser();

        private:
            Parser(Parser &p){}
            void operator=(Parser &p){}

            pANTLR3_UINT8               fName;
            pANTLR3_INPUT_STREAM        input;
            pMicroJavaLexer             lxr;
            pANTLR3_COMMON_TOKEN_STREAM tstream;
            pMicroJavaParser            psr;
            AST                         ast;

    };

}

#endif //PARSER_H

