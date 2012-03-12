
#include "parser/parser.h"

#define INDENT_INC 2

using namespace MJ;

const ANTLR3_UINT32 special_tokens[] = {PROGRAM, DEF, IF, WHILE};
const int SPECIAL_SIZE = sizeof(special_tokens) / sizeof(ANTLR3_UINT32);

void _pp_special(pANTLR3_BASE_TREE node, int indent);

bool isSpecial(AST node) {
    pANTLR3_COMMON_TOKEN token = node->getToken(node);
    ANTLR3_UINT32 type = token->getType(token);
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

    pANTLR3_COMMON_TOKEN token = node->getToken(node);

    int childCount = node->getChildCount(node);
    if (node->isNilNode(node) == ANTLR3_FALSE
            && childCount > 0) {
        printf(" (");
    } else {
        printf(" ");
    }

    printf("%s", token->getText(token)->chars);

    for(int i = 0; i < childCount; i++) {
        _pp((AST)node->getChild(node, i), indent);
    }

    if (node->isNilNode(node) == ANTLR3_FALSE
            && childCount > 0) {
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

    printf("(%s", node->getText(node)->chars);
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

    AST ast = parse(filename);

    pp(ast);
}

