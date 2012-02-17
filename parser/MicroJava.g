grammar MicroJava;

options {
    output=AST;
    language=C;
}

tokens {
	PROGRAM;
	DEFN;
	DEFCLASS;
	DEFARR;
	DEFVAR;
	DEFCONST;
	
	VAR;
	ARR;
	ARR_NEW;
	CLASS_NEW;
	
	FORMAL_PARAMS;
	CONDITION;
	IF;
	WHILE;
	BREAK;
	RETURN;
	EXPR;
	BLOCK;
	READ;
	PRINT;
	CALL;
	INC;
	DEC;
	NEG;
}


@header {
}

@lexer::header{
}

program :	'class' IDENT (const_decl|var_decl|class_decl)* '{' method_decl* '}'
			-> ^(PROGRAM const_decl* class_decl* var_decl* method_decl*);

const_decl
	:	'final' type IDENT '=' literal ';' -> ^(DEFCONST type IDENT literal);
	
literal :	NUMBER | CHAR ;
	

var_decl:	type IDENT ('[' ']' -> ^(DEFARR type IDENT)
				| -> ^(DEFVAR type IDENT))
			 (',' IDENT ('[' ']' -> ^(DEFARR type IDENT)
				| -> ^(DEFVAR type IDENT)))* ';';


class_decl
	:	'class' IDENT '{' var_decl* '}' -> ^(DEFCLASS IDENT var_decl*);

method_decl
	:	('void'|type) IDENT '(' formal_params? ')' var_decl* '{' (statement)* '}';

type 	:	IDENT ;

formal_params
	:	type IDENT('[' ']')? (',' type IDENT('[' ']')?)*;

statement
	:	while_stat
	|	'break' ';'!
	|	'return' expr ';'!
	|	'read' '('! designator ')'! ';'!
	|	'print' '('! expr (','! NUMBER)? ')'! ';'! 
	|	des_stat
	|	'{' statement* '}' -> ^(BLOCK statement*)
	| 	if_stat;
	
if_stat	:	'if' '(' condition ')' ifStatement=statement 
			('else' elseStatement=statement -> ^(IF condition $ifStatement $elseStatement)
			| -> ^(IF condition $ifStatement));
			
while_stat	:	'while' '(' condition ')' statement -> ^(WHILE condition statement);

des_stat
	:	designator (
			'=' expr ';' /* -> ^('=' designator expr) */
			| '(' actual_params? ')' ';' -> ^(CALL designator actual_params?)
			| '++' ';' -> ^(INC designator)
			| '--' ';' -> ^(DEC designator));
	
actual_params
	:	expr (',' expr)*;
	
condition
	:	condition_term ('||' condition_term)* /*-> ^('||' condition_term*)*/;

condition_term
	:	condition_fact ('&&' condition_fact)* /*-> ^('&&' condition_fact*)*/;

condition_fact
	:	expr RELOP expr /*-> ^(RELOP expr expr)*/;
	
expr	:	term (ADDOP term)* 
		| '-' expr;

term	:	factor (MULOP factor)*;

factor	:	designator ('(' actual_params? ')')?
	|	NUMBER
	|	CHAR
	|	'new' type ('['expr']')?
	|	'('! expr ')'!;

designator
	:	IDENT ('.' IDENT | '[' expr ']')*;
	
RELOP	:	'=='|'!='|'>'|'>='|'<'|'<=';

ADDOP	:	'+'|'-';

MULOP	:	'*'|'/'|'%';

IDENT 	:	('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'_'|'0'..'9')*;

NUMBER	:	('0'..'9')+;

CHAR	:	'\'' (' '..'~') '\'';

COMMENT
    : '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;};
    
WS	:	(' '|'\t'|'\r'|'\n')+ {$channel=HIDDEN;};
