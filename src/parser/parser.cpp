
#include "parser.h"
#include <cassert>

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


