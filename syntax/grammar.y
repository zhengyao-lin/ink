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
	Ink_ParamList *parameter;
	Ink_ExpressionList *expression_list;
	std::string *string;
	int token;
}

%token <string> TIDENTIFIER TINTEGER TFLOAT TSTRING

%token <token> TVAR TNULL TUNDEFINED TRETURN TNEW TCLONE
%token <token> TCOMMA TSEMICOLON TASSIGN
%token <token> TADD TSUB TMUL TDIV TMOD TDOT
%token <token> TLPAREN TRPAREN TLBRAKT TRBRAKT TLBRACE TRBRACE

%type <expression> expression assignment_expression
				   primary_expression postfix_expression
				   function_expression additive_expression
				   return_expression multiplicative_expression
				   unary_expression nestable_expression
%type <parameter> param_list param_opt
%type <expression_list> expression_list expression_list_opt
						argument_list argument_list_opt
						block

%start compile_unit

%%

compile_unit
	: expression_list_opt
	{
		exp_list = *$1;
		delete $1;
	}
	;

expression
	: nestable_expression
	| return_expression
	;

nestable_expression
	: assignment_expression
	;

return_expression
	: TRETURN
	{
		$$ = new Ink_ReturnExpression(NULL);
	}
	| TRETURN nestable_expression
	{
		$$ = new Ink_ReturnExpression($2);
	}
	;

assignment_expression
	: additive_expression
	| additive_expression TASSIGN assignment_expression
	{
		$$ = new Ink_AssignmentExpression($1, $3);
	}
	;

additive_expression
	: multiplicative_expression
	| additive_expression TADD multiplicative_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($3);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("+")), arg);
	}
	| additive_expression TSUB multiplicative_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($3);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("-")), arg);
	}
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression TMUL unary_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($3);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("*")), arg);
	}
	| multiplicative_expression TDIV unary_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($3);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("/")), arg);
	}
	;

argument_list
	: nestable_expression
	{
		$$ = new Ink_ExpressionList();
		$$->push_back($1);
	}
	| argument_list TCOMMA nestable_expression
	{
		$1->push_back($3);
		$$ = $1;
	}
	;

argument_list_opt
	: /* empty */
	{
		$$ = new Ink_ExpressionList();
	}
	| argument_list
	;

block
	: TLBRACE expression_list_opt TRBRACE
	{
		$$ = $2;
	}
	;

unary_expression
	: postfix_expression
	| TNEW postfix_expression TLPAREN argument_list_opt TRPAREN
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($2, new string("new")),
									*$4, true);
		delete $4;
	}
	| TCLONE unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($2, new string("clone")),
									Ink_ExpressionList());
	}
	| TADD unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($2, new string("+u")),
									Ink_ExpressionList());
	}
	| TSUB unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($2, new string("-u")),
									Ink_ExpressionList());
	}
	;

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
	| postfix_expression TLBRAKT argument_list TRBRAKT
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("[]")), *$3);
		delete $3;
	}
	| postfix_expression block
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back(new Ink_FunctionExpression(Ink_ParamList(), *$2, true));
		$$ = new Ink_CallExpression($1, arg);
		delete $2;
	}
	;

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
	;

param_opt
	: /* empty */
	{
		$$ = new Ink_ParamList();
	}
	| param_list
	;

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
	;

expression_list_opt
	: /* empty */
	{
		$$ = new Ink_ExpressionList();
	}
	| expression_list
	;

function_expression
	: primary_expression
	| TLPAREN param_opt TRPAREN block
	{
		$$ = new Ink_FunctionExpression(*$2, *$4);
		delete $2;
		delete $4;
	}
	;

primary_expression
	: TINTEGER
	{
		// printf("integer: %s\n", $1->c_str());
		$$ = Ink_IntegerConstant::parse(*$1);
		delete $1;
	}
	| TSTRING
	{
		$$ = new Ink_StringConstant($1);
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
	| TUNDEFINED
	{
		$$ = new Ink_UndefinedConstant();
	}
	| TLPAREN nestable_expression TRPAREN
	{
		$$ = $2;
	}
	;

%%