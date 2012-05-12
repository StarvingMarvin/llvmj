
#include "parser/parser.h"

#define INDENT_INC 2

using namespace mj;

const uint32_t special_tokens[] = {PROGRAM, DEF, IF, WHILE};
const size_t SPECIAL_SIZE = sizeof(special_tokens) / sizeof(size_t);

void _pp_special(AST node, int indent);

bool isSpecial(AST node) {
    uint32_t type = tokenType(node);
    for (int i = 0; i < SPECIAL_SIZE; i++) {
        if (special_tokens[i] == type) {
            return true;
        }
    }

    return false;
}

void _pp(AST node, int indent) {

    if (isSpecial(node)) {
        _pp_special(node, indent);
        return;
    }

    bool _nilNode = nilNode(node);

    if (!_nilNode && childCount > 0) {
        printf(" (");
    } else {
        printf(" ");
    }

    printf("%s", tokenText(node));

    for(nodeiterator bi = begin(node); bi <= end(node); bi++) {
        _pp(*bi, indent);
    }

    if (!_nilNode && childCount > 0) {
        printf(")");
    }

}

void _indent(int ammount) {
    printf("\n");
    for (int i = 0; i < ammount; i++) {
        printf(" ");
    }
}

void _pp_special(AST node, int indent) {
    _indent(indent);

    pANTLR3_STRING txt = node->getText(node);
    printf("(%s", txt->chars);
    int childCount = node->getChildCount(node);
    if (childCount > 0) {
        _pp((pANTLR3_BASE_TREE)node->getChild(node, 0), indent + 2* INDENT_INC);
    }

    int newIndent = indent + INDENT_INC;
    for (int i = 1; i < childCount; i++) {
        _indent(newIndent);
        _pp((pANTLR3_BASE_TREE)node->getChild(node, i), newIndent);
    }

    printf(")");

}

void pp(AST root) {
    _pp(root, 0);
}

int main(int argc, char** argv) {

    const char *filename = (argc < 2 || argv[1] == NULL)?
                        "./input" : argv[1];

    Parser p(filename);
    AST ast = p.parse();

    pp(ast);
}

