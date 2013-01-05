
#include "parser/parser.h"
#include <cstdio>

#define INDENT_INC 2

using namespace mj;


class PrintVisitor : public NodeVisitor {
    public:
        virtual void operator()(AstWalker &walker) const {

            bool nn = walker.nilNode();
            size_t cc = walker.childCount();

            if (!nn && cc > 0) {
                printf(" (");
            } else {
                printf(" ");
            }

            printf("%s", walker.tokenText());

            int indent = *(walker.getData<int>());
            for(nodeiterator bi = walker.firstChild(); 
                    bi < walker.lastChild(); bi++) {
                AST child = *bi;
                setNodeData<int>(child, &indent);
                walker.visit(child);
            }

            if (!nn && cc > 0) {
                printf(")");
            }

        }
};



class PrintSpecialVisitor : public NodeVisitor {
    public:
        virtual void operator()(AstWalker &walker) const {
            int indent = *(walker.getData<int>());
            _indent(indent);

            printf("(%s", walker.tokenText());
            nodeiterator bi = walker.firstChild();
            int newIndent = indent + INDENT_INC;
            int firstChildIndent = indent + 2 * INDENT_INC;
            
            if (bi != walker.lastChild()) {
                AST child = *bi;
                setNodeData<int>(child, &firstChildIndent);
                walker.visit(child);
                bi++;
            }

            for(; bi != walker.lastChild(); bi++) {
                AST child = *bi;
                _indent(newIndent);
                setNodeData<int>(child, &newIndent);
                walker.visit(child);
            }

            printf(")");
        }

    private:
        void _indent(int ammount) const {
            printf("\n");
            for (int i = 0; i < ammount; i++) {
                printf(" ");
            }
        }
};

const uint32_t special_tokens[] = 
    {PROGRAM, DEFFN, DEFVAR, DEFCLASS, DEFARR, DEFCONST, IF, WHILE, BLOCK};
const size_t SPECIAL_SIZE = sizeof(special_tokens) / sizeof(uint32_t);

int main(int argc, char** argv) {

    const char *filename = (argc < 2 || argv[1] == NULL)?
                        "./input" : argv[1];

    Parser p(filename);
    AST ast = p.parse();

    AstWalker walker(ast, PrintVisitor());
    for (size_t i = 0; i < SPECIAL_SIZE; i++) {
        walker.addVisitor(special_tokens[i], PrintSpecialVisitor());
    }
    int indent = 0;
    setNodeData<int>(ast, &indent);
    walker.walkTree();
}

