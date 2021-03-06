grammar MicroJava;

options {
    output=AST;
    language=C;
}

tokens {
    PROGRAM;
    DEFFN;
    DEFCLASS;
    DEFVAR;
    DEFCONST;
    DEFARR;
    NEW;
    NEW_ARR;

    CALL;
    LIT_INT;
    LIT_CHAR;
    BLOCK;
    VAR_DES;
    FIELD_DES;
    ARR_DES;
    UNARY_MINUS;
    DEREF;

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
    DOT      = '.';
    ARR_INDEX    = '[';
}


program : CLASS IDENT (const_decl|var_decl|class_decl)* '{' method_decl* '}'
            -> ^(PROGRAM IDENT const_decl* class_decl* var_decl* method_decl*);

const_decl
    :   FINAL type IDENT '=' (val=NUMBER | val=CHAR) ';'
        -> ^(DEFCONST type IDENT $val);

literal : NUMBER -> ^(LIT_INT NUMBER)
        | CHAR -> ^(LIT_CHAR CHAR);

var_decl: type IDENT ('[' ']' -> ^(DEFARR type IDENT )
                            | -> ^(DEFVAR type IDENT ))
            (',' IDENT ('[' ']' -> ^(DEFARR type IDENT)
                            | -> ^(DEFVAR type IDENT)))* ';';

class_decl
    :   CLASS IDENT '{' var_decl* '}' -> ^(DEFCLASS IDENT var_decl*);

method_type
    :   VOID | type;

method_decl
    :   method_type IDENT '(' formal_params? ')' var_decl* '{' statement* '}'
            -> ^(DEFFN method_type IDENT 
                    ^(BLOCK formal_params?) 
                    var_decl* 
                    statement*);

type    : IDENT ;

formal_param
    :   type IDENT 
            ('[' ']' -> ^(DEFARR type IDENT) 
            | -> ^(DEFVAR type IDENT));

formal_params
    :   formal_param (',' formal_param)* -> formal_param+;

statement
    :   while_stat
        | BREAK ';'!
        | RETURN^ expr? ';'!
        | READ^ '('! designator ')'! ';'!
        | print_stat
        | des_stat
        | '{' statement* '}' -> ^(BLOCK statement*)
        | if_stat;

print_stat
    :   PRINT '(' expr (',' NUMBER -> ^(PRINT expr ^(LIT_INT NUMBER))
                        | -> ^(PRINT expr)) ')' ';';

if_stat : IF^ '('! condition ')'! statement (ELSE! statement)?;

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
        | MINUS expr -> ^(UNARY_MINUS expr);

term    : factor (mulop^ factor)*;

factor  : designator ('(' actual_params? ')' 
                          -> ^(CALL designator actual_params?) 
                        | -> ^(DEREF designator))
        | literal
        | NEW type ('['expr']'-> ^(NEW_ARR type expr) | -> ^(NEW type))
        |   '('! expr ')'!;

designator
    :   var=IDENT ((DOT field=IDENT -> ^(FIELD_DES $var $field) 
                    | ARR_INDEX expr ']' -> ^(ARR_DES $var expr))+ 
            | -> ^(VAR_DES $var));


addop   :   PLUS | MINUS;

relop   :   EQL | NEQ | LST | LSE | GRT | GRE;

mulop   :   MUL | DIV | MOD;

IDENT   :   ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'_'|'0'..'9')*;

NUMBER  :   ('0'..'9')+;

CHAR    :   '\'' PRINTABLE_CHAR '\'' 
            {SETTEXT($PRINTABLE_CHAR->getText($PRINTABLE_CHAR));};

fragment
PRINTABLE_CHAR
    :   ' '..'~';

COMMENT
    : '//' ~('\n'|'\r')* '\r'? '\n' {SKIP();};
    
WS	:	(' '|'\t'|'\r'|'\n')+ {SKIP();};

