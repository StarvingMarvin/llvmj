
#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <string>

namespace mj {
    struct Token {
        std::string name;
        std::string text;
        int line;
        int col;
    }
}

#endif // _TOKEN_H_

