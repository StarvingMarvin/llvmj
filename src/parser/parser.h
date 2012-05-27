
#ifndef PARSER_H
#define PARSER_H

#include "parser/MicroJavaLexer.h"
#include "parser/MicroJavaParser.h"
#include <string>
#include <sys/types.h>
#include <iterator>
#include <map>
#include <vector>

namespace mj {

    typedef pANTLR3_BASE_TREE AST;

    class AstWalker;

    class NodeVisitor {
        public: 
            virtual void operator()(AstWalker *walker);
    };

    class nodeiterator : 
        public std::iterator<std::random_access_iterator_tag, AST, int, AST*, AST>
    {
        AST _parent;
        size_t _index;
        public:
        nodeiterator(AST parent) :
            _parent(parent), _index(0) {}
        nodeiterator(AST parent, size_t index) :
            _parent(parent), _index(index) {}

        nodeiterator& operator++() {_index++; return *this;}
        nodeiterator operator++(int) {nodeiterator tmp(*this); operator++(); return tmp;}
        nodeiterator& operator--() {_index--; return *this;}
        nodeiterator operator--(int) {nodeiterator tmp(*this); operator--(); return tmp;}
        nodeiterator operator+(int i) {return nodeiterator(_parent, _index + i);}
        nodeiterator operator-(int i) {return nodeiterator(_parent, _index - i);}
        nodeiterator& operator+=(int i) {_index+=i; return *this;}
        nodeiterator& operator-=(int i) {_index-=i; return *this;}
        int operator-(const nodeiterator &rhs) {return _index - rhs._index;}
        bool operator==(const nodeiterator &rhs) {return _index == rhs._index;}
        bool operator!=(const nodeiterator &rhs) {return _index != rhs._index;}
        bool operator>(const nodeiterator &rhs) {return _index > rhs._index;}
        bool operator<(const nodeiterator &rhs) {return _index < rhs._index;}
        bool operator>=(const nodeiterator &rhs) {return _index <= rhs._index;}
        bool operator<=(const nodeiterator &rhs) {return _index >= rhs._index;}
        AST operator*() {return (AST)_parent->getChild(_parent, _index);}
        AST operator[](int i) {return (AST)_parent->getChild(_parent, i);}

    };

    class AstWalker {
        public:
            AstWalker(AST ast, NodeVisitor* defaultVisitor = new NodeVisitor());
            void addVisitor(uint32_t tokenType, NodeVisitor* visitor);
            NodeVisitor* getVisitor(uint32_t tokenType);
            void visit(AST ast);
            void walkTree();
            uint32_t tokenType();
            char* tokenText();
            size_t childCount();
            bool nilNode();
            nodeiterator firstChild();
            nodeiterator lastChild();
            AST currentNode();
            template <class T>
            void setData(T* data) {
                stack.back()->u = data;
            }

            template <class T>
            T* getData() {
                return static_cast<T*>(stack.back()->u);
            }
        private:
            NodeVisitor* _defaultVisitor;
            std::map<uint32_t, NodeVisitor*> visitors;
            std::vector<AST> stack;
    };


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


    uint32_t tokenType(AST ast);

    char* tokenText(AST ast);

    size_t childCount(AST ast);

    AST parentNode(AST ast);

    bool nilNode(AST ast);

    nodeiterator begin(AST parent);
    nodeiterator end(AST parent);

    template <class T>
    void setNodeData(AST ast, T* data) {
        ast->u = data;
    }

    template <class T>
    T* getNodeData(AST ast) {
        return static_cast<T*>(ast->u);
    }

}

#endif //PARSER_H

