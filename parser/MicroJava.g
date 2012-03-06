grammar MicroJava;

options {
    output=AST;
    language=C;
}

tokens {
    PROGRAM;
    DEF;
    FN;
    CLASS;
    VAR;
    CONST;
    ARR;
    LIT;

    CALL;
    INT_T;
    CHAR_T;

    CLASS   = 'class';
    NEW     = 'new';
    FINAL   = 'final';
    VOID    = 'void';
    IF      = 'if';
    ELSE    = 'else';
    WHILE   = 'while';
    BREAK   = 'break';
    PRINT   = 'print';
    READ    = 'read';
    RETURN  = 'return';

    PLUS    = '+';
    MINUS   = '-';
    INC     = '++';
    DEC     = '--';
    SET     = '=';
    OR      = '||';
    AND     = '&&';

    MUL     = '*';
    DIV     = '/';
    MOD     = '%';

    EQL      = '==';
    NEQ      = '!=';
    GRT      = '>';
    GRE      = '>=';
    LST      = '<';
    LSE      = '<=';
}


@header {
}

@lexer::header{
}

program : CLASS IDENT (const_decl|var_decl|class_decl)* '{' method_decl* '}'
            -> ^(PROGRAM IDENT const_decl* class_decl* var_decl* method_decl*);

const_decl
    :   FINAL type IDENT '=' literal ';' -> ^(DEF ^(CONST type IDENT) literal);

literal : NUMBER -> ^(LIT INT_T NUMBER)
        | CHAR -> ^(LIT CHAR_T CHAR);

var_decl: type IDENT ('[' ']' -> ^(DEF ^(ARR type IDENT))
                | -> ^(DEF ^(VAR type IDENT)))
            (',' IDENT ('[' ']' -> ^(DEF ^(ARR type IDENT))
                | -> ^(DEF ^(VAR type IDENT))))* ';';


class_decl
    :   CLASS IDENT '{' var_decl* '}' -> ^(DEF ^(CLASS IDENT) var_decl*);

method_type
    :   VOID | type;

method_decl
    :   method_type IDENT '(' formal_params? ')' var_decl* '{' statement* '}'
            -> ^(DEF ^(FN method_type IDENT formal_params?) var_decl* statement*);

type    : IDENT ;

formal_param
    :   type IDENT 
            ('[' ']' -> ^(DEF ^(ARR type IDENT)) 
            | -> ^(DEF ^(VAR type IDENT)));

formal_params
    :   formal_param (',' formal_param)* -> formal_param+;

statement
    :   while_stat
        | BREAK ';'!
        | RETURN^ expr? ';'!
        | READ^ '('! designator ')'! ';'!
        | PRINT^ '('! expr (','! NUMBER)? ')'! ';'! 
        | des_stat
        | '{' statement* '}' -> ^(statement)*
        | if_stat;

if_stat : IF '(' condition ')' ifStatement=statement 
            (ELSE elseStatement=statement -> ^(IF condition $ifStatement $elseStatement)
            | -> ^(IF condition $ifStatement));

while_stat
    :   WHILE^ '('! condition ')'! statement;

des_stat
    :   designator (
            SET expr ';'  -> ^(SET designator expr)
            | '(' actual_params? ')' ';' -> ^(CALL designator actual_params?)
            | INC ';' -> ^(INC designator)
            | DEC ';' -> ^(DEC designator));

actual_params
    :   expr (','! expr)*;

condition
    :   condition_term (OR^ condition_term)*;

condition_term
    :   condition_fact (AND^ condition_fact)*;

condition_fact
    :   expr relop^ expr;

expr    :   term (addop^ term)*
        | MINUS^ expr;

term    : factor (mulop^ factor)*;

factor  : designator ('(' actual_params? ')' -> ^(CALL designator actual_params?) 
                        | -> designator)
        | literal
        | NEW type ('['expr']'-> ^(NEW ^(ARR type expr)) | -> ^(NEW ^(VAR type)))
        |   '('! expr ')'!;

designator
    :   IDENT ('.'^ IDENT | '['^ expr ']'!)*;


addop   :   PLUS | MINUS;

relop   :   EQL | NEQ | LST | LSE | GRT | GRE;

mulop   :   MUL | DIV | MOD;

IDENT   :   ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'_'|'0'..'9')*;

NUMBER  :   ('0'..'9')+;

CHAR    :   '\'' PRINTABLE_CHAR '\'' {SETTEXT($PRINTABLE_CHAR->getText($PRINTABLE_CHAR));};

fragment
PRINTABLE_CHAR
    :   ' '..'~';

COMMENT
    : '//' ~('\n'|'\r')* '\r'? '\n' {SKIP();};
    
WS	:	(' '|'\t'|'\r'|'\n')+ {SKIP();;};

