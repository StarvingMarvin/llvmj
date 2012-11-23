
#include "parser.h"
#include <cassert>
#include <iostream>

using namespace mj;
using std::string;

Parser::Parser(const string filename):
    input(0),
    lxr(0),
    tstream(0),
    psr(0),
    ast(0)
{
    
    fName = (pANTLR3_UINT8) filename.c_str();
}

AST Parser::parse() throw(ParserException) {
    if (ast != NULL) {
        return ast;
    }

    input	= antlr3AsciiFileStreamNew(fName);

    if (input == NULL)
    {
        fprintf(stderr, "Unable to open file %s\n", (char *)fName);
        throw ParserException("Unable to open file.");
    }

    lxr = MicroJavaLexerNew(input);

    if ( lxr == NULL )
    {
        fprintf(stderr, "Unable to create the lexer due to malloc() failure!\n");
        throw ParserException("Unable to create lexer.");
    }

    tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));

    if (tstream == NULL)
    {
        fprintf(stderr, "Out of memory trying to allocate token stream\n");
        throw ParserException("Out of memory trying to allocate token stream.");
    }

    psr = MicroJavaParserNew(tstream);

    if (psr == NULL)
    {
        fprintf(stderr, "Out of memory trying to allocate parser\n");
        throw ParserException("Out of memory trying to allocate parser");
    }

    ast = psr->program(psr).tree;

    if (psr->pParser->rec->state->errorCount > 0) {
        fprintf(stderr, "The parser returned %d errors, tree walking aborted.\n", psr->pParser->rec->state->errorCount);
    }
    return ast;
}

Parser::~Parser() {

    if (psr != NULL) {
        psr->free(psr);
        psr = NULL;
    }

    if (tstream != NULL) {
        tstream->free(tstream);
        tstream = NULL;
    }

    if (lxr != NULL) {
        lxr->free(lxr);
        lxr = NULL;
    }

    if (input != NULL) {
        input->close(input);
        input = NULL;
    }
}

uint32_t mj::tokenType(AST ast) {
    pANTLR3_COMMON_TOKEN token = ast->getToken(ast);
    return (uint32_t)token->getType(token);
}

char* mj::tokenText(AST ast) {
    pANTLR3_COMMON_TOKEN token = ast->getToken(ast);
    return (char*) token->getText(token)->chars;
}

size_t mj::childCount(AST ast) {
    return ast->getChildCount(ast);
}

AST mj::parentNode(AST ast) {
    return ast->getParent(ast);
}

bool mj::nilNode(AST ast) {
    return ast->isNilNode(ast);
}

std::ostream& mj::printPosition(std::ostream& os, AST ast) {
    os << "line " << ast->getLine(ast) 
        << ", column " << ast->getCharPositionInLine(ast);
    return os;
}

nodeiterator mj::begin(AST parent) {
    return nodeiterator(parent);
}

nodeiterator mj::end(AST parent) {
    return nodeiterator(parent, childCount(parent));
}

void NodeVisitor::operator()(AstWalker &walker) const {}

AstWalker::AstWalker(AST ast, const NodeVisitor &defaultVisitor):
    _defaultVisitor(defaultVisitor),
    stack()
{
    stack.push_back(ast);
}

void AstWalker::addVisitor(uint32_t tokenType, const NodeVisitor &visitor) {
    visitors[tokenType]=const_cast<NodeVisitor*>(&visitor);
}

const NodeVisitor& AstWalker::getVisitor(uint32_t tokenType) {
    const NodeVisitor* visitor = visitors[tokenType];
    if (!visitor) {
        return _defaultVisitor;
    }
    return *visitor;
}

void AstWalker::visit(AST ast) {
    stack.push_back(ast);
#ifdef DEBUG
    std::cout<< "Visiting " << tokenText() << std::endl;
#endif
    uint32_t tt = tokenType();
    const NodeVisitor& visitor = getVisitor(tt);
    visitor(*this);
    stack.pop_back();
}

uint32_t AstWalker::tokenType() {
    return mj::tokenType(stack.back());
}

char* AstWalker::tokenText() {
    return mj::tokenText(stack.back());
}

size_t AstWalker::childCount() {
    return mj::childCount(stack.back());
}

bool AstWalker::nilNode() {
    return mj::nilNode(stack.back());
}

std::ostream& AstWalker::printPosition(std::ostream& os) {
    return mj::printPosition(os, stack.back());
}

nodeiterator AstWalker::firstChild() {
    return begin(stack.back());
}

nodeiterator AstWalker::lastChild() {
    return end(stack.back());
}

AST AstWalker::currentNode() {
    return stack.back();
}

void AstWalker::walkTree() {
    visit(stack.back());
}

