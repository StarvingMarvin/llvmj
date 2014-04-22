
#include "parser/parser.h"

#define INDENT_INC 2

using namespace mj;

namespace mj {

class PrintVisitor : public NodeVisitor {
    public:
        PrintVisitor(std::ostream &os): out(os){}

        virtual void operator()(AstWalker &walker) const {

            bool nn = walker.nilNode();
            size_t cc = walker.childCount();

            if (!nn && cc > 0) {
                out << " (";
            } else {
                out << " ";
            }

            out <<  walker.tokenText();

            int indent = *(walker.getData<int>());
            for(nodeiterator bi = walker.firstChild(); 
                    bi < walker.lastChild(); bi++) {
                AST child = *bi;
                setNodeData<int>(child, &indent);
                walker.visit(child);
            }

            if (!nn && cc > 0) {
                out << ")";
            }

        }
    protected:
        std::ostream &out;
};


class PrintSpecialVisitor : public PrintVisitor {
    public:
    PrintSpecialVisitor(std::ostream &os): PrintVisitor(os){}
        virtual void operator()(AstWalker &walker) const {
            int indent = *(walker.getData<int>());
            _indent(indent);

            out << "(" << walker.tokenText();
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

            out << ")";
        }

    private:
        void _indent(int ammount) const {
            out << std::endl;
            for (int i = 0; i < ammount; i++) {
                out << " ";
            }
        }

};

const uint32_t special_tokens[] = 
    {PROGRAM, DEFFN, DEFVAR, DEFCLASS, DEFARR, DEFCONST, IF, WHILE, BLOCK};
const size_t SPECIAL_SIZE = sizeof(special_tokens) / sizeof(uint32_t);

void pretty_print(AST ast, std::ostream &os) {
    PrintVisitor print(os);
    AstWalker walker(print);
    PrintSpecialVisitor special(os);
    for (size_t i = 0; i < SPECIAL_SIZE; i++) {
        walker.addVisitor(special_tokens[i], special);
    }
    int indent = 0;
    setNodeData<int>(ast, &indent);
    walker.visit(ast);
    os << std::endl;
}

}


