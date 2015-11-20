%{
    #include <stdio.h>
    #include <stdlib.h>
	#include <string.h>
	#include "core/expression.h"

	extern Ink_ExpressionList exp_list;

	extern int yylex();
	void yyerror(const char *msg) {
		printf("%s\n", msg);
	}
%}

%union {
	Ink_Expression *expression;
	Ink_Assignable *assignable;
	Ink_ParamList *parameter;
	Ink_ExpressionList *expression_list;
	std::string *string;
	int token;
}

%token <string> TIDENTIFIER TINTEGER TFLOAT

%token <token> TVAR TNULL
%token <token> TCOMMA TSEMICOLON TASSIGN
%token <token> TADD TSUB TMUL TDIV TMOD TDOT
%token <token> TLPAREN TRPAREN TLBRAKT TRBRAKT TLBRACE TRBRACE

%type <expression> expression assignment_expression
				   primary_expression postfix_expression
				   function_expression additive_expression
%type <assignable> assignable_expression
%type <parameter> param_list param_opt
%type <expression_list> expression_list expression_list_opt
						argument_list argument_list_opt

%start compile_unit

%%

compile_unit
	: expression_list_opt
	{
		exp_list = *$1;
		delete $1;
	}

expression
	: assignment_expression

assignable_expression
	: TIDENTIFIER
	{
		$$ = new Ink_Assignable($1);
	}
	| assignable_expression TDOT TIDENTIFIER
	{
		$$ = new Ink_Assignable($1, $3);
	}

assignment_expression
	: additive_expression
	| additive_expression TASSIGN assignment_expression
	{
		$$ = new Ink_AssignmentExpression($1, $3);
	}

additive_expression
	: postfix_expression
	| additive_expression TADD postfix_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($3);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("+")), arg);
	}

argument_list
	: expression
	{
		$$ = new Ink_ExpressionList();
		$$->push_back($1);
	}
	| argument_list TCOMMA expression
	{
		$1->push_back($3);
		$$ = $1;
	}

argument_list_opt
	: /* empty */
	{
		$$ = new Ink_ExpressionList();
	}
	| argument_list

postfix_expression
	: function_expression
	| postfix_expression TDOT TIDENTIFIER
	{
		$$ = new Ink_HashExpression($1, new string($3->c_str()));
		delete $3;
	}
	| postfix_expression TLPAREN argument_list_opt TRPAREN
	{
		$$ = new Ink_CallExpression($1, *$3);
		delete $3;
	}

param_list
	: TIDENTIFIER
	{
		$$ = new Ink_ParamList();
		$$->push_back($1);
	}
	| param_list TCOMMA TIDENTIFIER
	{
		$1->push_back($3);
		$$ = $1;
	}

param_opt
	: /* empty */
	{
		$$ = new Ink_ParamList();
	}
	| param_list

expression_list
	: expression TSEMICOLON
	{
		$$ = new Ink_ExpressionList();
		$$->push_back($1);
	}
	| expression_list expression TSEMICOLON
	{
		$1->push_back($2);
		$$ = $1;
	}

expression_list_opt
	: /* empty */
	{
		$$ = new Ink_ExpressionList();
	}
	| expression_list

function_expression
	: primary_expression
	| TLPAREN param_opt TRPAREN TLBRACE expression_list_opt TRBRACE
	{
		$$ = new Ink_FunctionExpression(*$2, *$5);
		delete $2;
		delete $5;
	}

primary_expression
	: TINTEGER
	{
		// printf("integer: %s\n", $1->c_str());
		$$ = Ink_IntegerConstant::parse($1, true);
		delete $1;
	}
	| TIDENTIFIER
	{
		$$ = new Ink_IdentifierExpression(new string($1->c_str()));
		delete $1;
	}
	| TNULL
	{
		$$ = new Ink_NullConstant();
	}
	| TLPAREN expression TRPAREN
	{
		$$ = $2;
	}


%%