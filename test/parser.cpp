
#include "MicroJavaLexer.h"
#include "MicroJavaParser.h"
#include <cassert>
#include <cstring>


class TestParser {
public:
    TestParser(const char* inputStr);
    virtual void printAST() = 0;
    virtual bool valid();
    virtual ~TestParser();
    pMicroJavaParser parser() { return psr; }
private:
    pANTLR3_INPUT_STREAM	    input;
    pMicroJavaLexer			    lxr;
    pANTLR3_COMMON_TOKEN_STREAM	    tstream;
    pMicroJavaParser			    psr;
};


TestParser::TestParser(const char* inputStr) {
    input = antlr3NewAsciiStringInPlaceStream 
                ((pANTLR3_UINT8)inputStr, (ANTLR3_UINT64) strlen(inputStr), NULL);

    lxr	    = MicroJavaLexerNew(input);
    tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));
    psr	    = MicroJavaParserNew(tstream);
    printf ("== Testing ==\n%s\n", inputStr);
}

bool TestParser::valid() {
    int errorCount = psr->pParser->rec->state->errorCount;
    printf("errorCount: %d\n", errorCount);
    return errorCount == 0;
}

TestParser::~TestParser() {
    psr	    ->free  (psr);		psr		= NULL;
    tstream ->free  (tstream);	tstream	= NULL;
    lxr	    ->free  (lxr);	    lxr		= NULL;
    input   ->close (input);	input	= NULL;
}

class DesignatorParser : public TestParser {
public:
    DesignatorParser(const char* inputStr);
    virtual void printAST();
private:
    MicroJavaParser_designator_return designatorAST;
};

DesignatorParser::DesignatorParser(const char* inputStr) 
    : TestParser(inputStr) {
    designatorAST = parser()->designator(parser());
}

void DesignatorParser::printAST() {
    assert (valid()); 
    printf("Nodes: %s\n", designatorAST.tree->
            toStringTree(designatorAST.tree)->chars);
}

void testDesignator() {
    DesignatorParser("a.b").printAST();
    DesignatorParser("a[b]").printAST();
    DesignatorParser("a.b[c]").printAST();
    DesignatorParser("a[b].c").printAST();
    DesignatorParser("a[b].c.d[1].e.f[2]").printAST();

    assert(!DesignatorParser("a.").valid());
    assert(!DesignatorParser("a..b").valid());
    assert(!DesignatorParser("a[b").valid());
    assert(!DesignatorParser("a[]").valid());
    assert(!DesignatorParser("[a]").valid());

}

class ExpressionParser : public TestParser {
public:
    ExpressionParser(const char* inputStr);
    virtual void printAST();
private:
    MicroJavaParser_expr_return expressionAST;
};

ExpressionParser::ExpressionParser(const char* inputStr)
    : TestParser(inputStr) {
    
    expressionAST = parser()->expr(parser());
}

void ExpressionParser::printAST() {
    assert(valid());
    printf("Nodes: %s\n", expressionAST.tree->
            toStringTree(expressionAST.tree)->chars);
}

void testExpression() {
    ExpressionParser(" 1 + 2").printAST();
    ExpressionParser(" 1 - 2").printAST();
    ExpressionParser(" 1 * 2").printAST();
    ExpressionParser(" 1 + 2 + 3").printAST();
    ExpressionParser(" 1 - 2 - 3").printAST();
    ExpressionParser(" a + 2 - 1").printAST();
    //ExpressionParser(" a + 2 * b").printAST();
    //ExpressionParser(" a * 2 * b").printAST();
    ExpressionParser("(1 + a) / 5").printAST();
    ExpressionParser("a[i] + 2").printAST();
    //ExpressionParser("1 - x.y").printAST();
    ExpressionParser("1 * 2 / 3 + f(4,2) * (a + b.x) / c[3] - 5").printAST();
}

class ConditionParser : public TestParser {
public:
    ConditionParser(const char* inputStr);
    virtual void printAST();
private:
    MicroJavaParser_condition_return conditionAST;
};

ConditionParser::ConditionParser(const char* inputStr) 
    : TestParser(inputStr) {
    conditionAST = parser()->condition(parser());
}

void ConditionParser::printAST() {
    //assert (valid()); 
    printf("Nodes: %s\n", conditionAST.tree->
            toStringTree(conditionAST.tree)->chars);
}
void testCondition() {
    ConditionParser("a > 0").printAST();
    ConditionParser("x == y + 1").printAST();
}

class StatementParser : public TestParser {
public:
    StatementParser(const char* inputStr) 
        : TestParser(inputStr) 
    {
        statementAST = parser()->statement(parser());
    }
    virtual void printAST(){
        assert (valid()); 
        printf("Nodes: %s\n", statementAST.tree->
                toStringTree(statementAST.tree)->chars);
    }
private:
    MicroJavaParser_statement_return statementAST;
};


void testStatement() {
    StatementParser("x = x + 1;").printAST();
    StatementParser("x = a - b;").printAST();
    StatementParser("while (a > 0) {}").printAST();
    StatementParser("if (x == y) {a = 0;} else {a = b;}").printAST();
    StatementParser("print(x, 3);").printAST();

}

void testProgram (const char* filename){
    // Name of the input file. Note that we always use the abstract type pANTLR3_UINT8
    pANTLR3_UINT8	    fName = (pANTLR3_UINT8)filename;

    // The ANTLR3 character input stream, which abstracts the input source such that
    // it is easy to provide input from different sources such as files, or 
    // memory strings.
    //
    // For an ASCII/latin-1 memory string use:
    //	    input = antlr3NewAsciiStringInPlaceStream (stringtouse, (ANTLR3_UINT64) length, NULL);
    //
    // For a UCS2 (16 bit) memory string use:
    //	    input = antlr3NewUCS2StringInPlaceStream (stringtouse, (ANTLR3_UINT64) length, NULL);
    //
    // For input from a file, see code below
    pANTLR3_INPUT_STREAM	    input;

    // The lexer is of course generated by ANTLR, and so the lexer type is not upper case.
    // The lexer is supplied with a pANTLR3_INPUT_STREAM from whence it consumes its
    // input and generates a token stream as output.
    //
    pMicroJavaLexer			    lxr;

    // The token stream is produced by the ANTLR3 generated lexer. Again it is a structure based
    // API/Object, which you can customise and override methods of as you wish. a Token stream is
    // supplied to the generated parser, and you can write your own token stream and pass this in
    // if you wish.
    //
    pANTLR3_COMMON_TOKEN_STREAM	    tstream;

    // The Lang parser is also generated by ANTLR and accepts a token stream as explained
    // above. The token stream can be any source in fact, so long as it implements the 
    // ANTLR3_TOKEN_SOURCE interface. In this case the parser does not return anything
    // but it can of course specify any kind of return type from the rule you invoke
    // when calling it.
    //
    pMicroJavaParser			    psr;

    // The parser produces an AST, which is returned as a member of the return type of
    // the starting rule (any rule can start first of course). This is a generated type
    // based upon the rule we start with.
    //
    MicroJavaParser_program_return	    langAST;

    // Finally, when the parser runs, it will produce an AST that can be traversed by the 
    // the tree parser: c.f. LangDumpDecl.g3t
    //
    //pLangDumpDecl		    treePsr;

    // Create the input stream based upon the argument supplied to us on the command line
    // for this example, the input will always default to ./input if there is no explicit
    // argument.
    //

    input	= antlr3AsciiFileStreamNew(fName);

    if (input == NULL)
    {
        fprintf(stderr, "Unable to open file %s\n", (char *)fName);
        exit(ANTLR3_ERR_NOMEM);
    }

    lxr	    = MicroJavaLexerNew(input);	    // CLexerNew is generated by ANTLR

    if ( lxr == NULL )
    {
        fprintf(stderr, "Unable to create the lexer due to malloc() failure1\n");
        exit(ANTLR3_ERR_NOMEM);
    }

    // Our lexer is in place, so we can create the token stream from it
    // NB: Nothing happens yet other than the file has been read. We are just 
    // connecting all these things together and they will be invoked when we
    // call the parser rule. ANTLR3_SIZE_HINT can be left at the default usually
    // unless you have a very large token stream/input. Each generated lexer
    // provides a token source interface, which is the second argument to the
    // token stream creator.
    // Note tha even if you implement your own token structure, it will always
    // contain a standard common token within it and this is the pointer that
    // you pass around to everything else. A common token as a pointer within
    // it that should point to your own outer token structure.
    //
    tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));

    if (tstream == NULL)
    {
        fprintf(stderr, "Out of memory trying to allocate token stream\n");
        exit(ANTLR3_ERR_NOMEM);
    }

    psr	    = MicroJavaParserNew(tstream);  // CParserNew is generated by ANTLR3

    if (psr == NULL)
    {
        fprintf(stderr, "Out of memory trying to allocate parser\n");
        exit(ANTLR3_ERR_NOMEM);
    }

    // So, we now invoke the parser. All elements of ANTLR3 generated C components
    // as well as the ANTLR C runtime library itself are pseudo objects. This means
    // that they are represented as pointers to structures, which contain any
    // instance data they need, and a set of pointers to other interfaces or
    // 'methods'. Note that in general, these few pointers we have created here are
    // the only things you will ever explicitly free() as everything else is created
    // via factories, that allocate memory efficiently and free() everything they use
    // automatically when you close the parser/lexer/etc.
    langAST = psr->program(psr);

    // If the parser ran correctly, we will have a tree to parse. In general I recommend
    // keeping your own flags as part of the error trapping, but here is how you can
    // work out if there were errors if you are using the generic error messages
    //
    if (psr->pParser->rec->state->errorCount > 0)
    {
        fprintf(stderr, "The parser returned %d errors, tree walking aborted.\n", psr->pParser->rec->state->errorCount);
        exit(1);
    }
    else
    {
        printf("Nodes: %s\n", langAST.tree->toStringTree(langAST.tree)->chars);

        // Tree parsers are given a common tree node stream (or your override)
        //
        //treePsr	= LangDumpDeclNew(nodes);

        //treePsr->decl(treePsr);
        //nodes   ->free  (nodes);	    nodes	= NULL;
        //treePsr ->free  (treePsr);	    treePsr	= NULL;
    }

    // We did not return anything from this parser rule, so we can finish. It only remains
    // to close down our open objects, in the reverse order we created them
    //
    psr	    ->free  (psr);		psr		= NULL;
    tstream ->free  (tstream);	tstream	= NULL;
    lxr	    ->free  (lxr);	    lxr		= NULL;
    input   ->close (input);	input	= NULL;

}

int main (int argc, char *argv[]) {

    testDesignator();
    testExpression();
    testCondition();
    testStatement();

    const char *fName = (argc < 2 || argv[1] == NULL)?
                        "./input" : argv[1];

    testProgram(fName);
    return 0;
}

