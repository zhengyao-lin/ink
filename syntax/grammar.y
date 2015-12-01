%{
    #include <stdio.h>
    #include <stdlib.h>
	#include <string.h>
	#include "core/expression.h"
	#define SET_LINE_NO(exp) (exp->line_number = current_line_number)

	extern Ink_ExpressionList exp_list;
	extern int current_line_number;

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
	IDContextType context_type;
	int token;
}

%token <string> TIDENTIFIER TINTEGER TFLOAT TSTRING

%token <token> TVAR TGLOBAL TLET TRETURN TNEW TCLONE
%token <token> TDNOT TNOT TCOMMA TSEMICOLON TCOLON TASSIGN
%token <token> TADD TSUB TMUL TDIV TMOD TDOT
%token <token> TLPAREN TRPAREN TLBRAKT TRBRAKT TLBRACE TRBRACE
%token <token> TARR TINS TCLT TCGT

%type <expression> expression assignment_expression
				   primary_expression postfix_expression
				   function_expression additive_expression
				   return_expression multiplicative_expression
				   unary_expression nestable_expression
				   insert_expression field_expression
				   table_expression
%type <parameter> param_list param_opt
%type <expression_list> expression_list expression_list_opt
						argument_list argument_list_opt
						block element_list element_list_opt
%type <context_type> id_context_type

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
	: field_expression
	;

field_expression
	: insert_expression
	| TIDENTIFIER TCOLON field_expression
	{
		$$ = new Ink_AssignmentExpression(
				 new Ink_HashExpression(
				 	 new Ink_IdentifierExpression(new string("this")),
				 	 $1),
				 $3);
		SET_LINE_NO($$);
	}

insert_expression
	: assignment_expression
	| insert_expression TINS assignment_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($3);

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("<<")), arg);
		SET_LINE_NO($$);
	}
	;

return_expression
	: TRETURN
	{
		$$ = new Ink_ReturnExpression(NULL);
		SET_LINE_NO($$);
	}
	| TRETURN nestable_expression
	{
		$$ = new Ink_ReturnExpression($2);
		SET_LINE_NO($$);
	}
	;

assignment_expression
	: additive_expression
	| additive_expression TASSIGN assignment_expression
	{
		$$ = new Ink_AssignmentExpression($1, $3);
		SET_LINE_NO($$);
	}
	| additive_expression TARR assignment_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($3);

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("->")), arg);
		SET_LINE_NO($$);
	}
	;

additive_expression
	: multiplicative_expression
	| additive_expression TADD multiplicative_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($3);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("+")), arg);
		SET_LINE_NO($$);
	}
	| additive_expression TSUB multiplicative_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($3);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("-")), arg);
		SET_LINE_NO($$);
	}
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression TMUL unary_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($3);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("*")), arg);
		SET_LINE_NO($$);
	}
	| multiplicative_expression TDIV unary_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($3);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("/")), arg);
		SET_LINE_NO($$);
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
	: table_expression
	| TNEW postfix_expression TLPAREN argument_list_opt TRPAREN
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($2, new string("new")),
									*$4);
		delete $4;
		SET_LINE_NO($$);
	}
	| TCLONE unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($2, new string("clone")),
									Ink_ExpressionList());
		SET_LINE_NO($$);
	}
	| TADD unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($2, new string("+u")),
									Ink_ExpressionList());
		SET_LINE_NO($$);
	}
	| TSUB unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($2, new string("-u")),
									Ink_ExpressionList());
		SET_LINE_NO($$);
	}
	| TDNOT unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($2, new string("!!")),
									Ink_ExpressionList());
		SET_LINE_NO($$);
	}
	;

element_list
	: nestable_expression
	{
		$$ = new Ink_ExpressionList();
		$$->push_back($1);
	}
	| element_list TCOMMA nestable_expression
	{
		$1->push_back($3);
		$$ = $1;
	}
	;

element_list_opt
	: /* empty */
	{
		$$ = new Ink_ExpressionList();
	}
	| element_list

table_expression
	: postfix_expression
	| TLBRACE element_list_opt TRBRACE
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression(
										new Ink_FunctionExpression(Ink_ParamList(), *$2),
										new string("new")),
									Ink_ExpressionList());
		delete $2;
		SET_LINE_NO($$);
	}

postfix_expression
	: function_expression
	| postfix_expression TDOT TIDENTIFIER
	{
		$$ = new Ink_HashExpression($1, new string($3->c_str()));
		delete $3;
		SET_LINE_NO($$);
	}
	| postfix_expression TLPAREN argument_list_opt TRPAREN
	{
		$$ = new Ink_CallExpression($1, *$3);
		delete $3;
		SET_LINE_NO($$);
	}
	| postfix_expression TLBRAKT argument_list TRBRAKT
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("[]")), *$3);
		delete $3;
		SET_LINE_NO($$);
	}
	| postfix_expression block
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back(new Ink_FunctionExpression(Ink_ParamList(), *$2, true));
		$$ = new Ink_CallExpression($1, arg);
		delete $2;
		SET_LINE_NO($$);
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
		SET_LINE_NO($$);
	}
	;

id_context_type
	: TLET
	{
		$$ = ID_LOCAL;
	}
	| TGLOBAL
	{
		$$ = ID_GLOBAL;
	}

primary_expression
	: TINTEGER
	{
		// printf("integer: %s\n", $1->c_str());
		$$ = Ink_IntegerConstant::parse(*$1);
		delete $1;
		SET_LINE_NO($$);
	}
	| TSTRING
	{
		$$ = new Ink_StringConstant($1);
		SET_LINE_NO($$);
	}
	| TIDENTIFIER
	{
		$$ = new Ink_IdentifierExpression($1);
		SET_LINE_NO($$);
	}
	| TVAR TIDENTIFIER
	{
		$$ = new Ink_IdentifierExpression($2, ID_COMMON, true);
		SET_LINE_NO($$);
	}
	| id_context_type TIDENTIFIER
	{
		$$ = new Ink_IdentifierExpression($2, $1);
		SET_LINE_NO($$);
	}
	| TVAR id_context_type TIDENTIFIER
	{
		$$ = new Ink_IdentifierExpression($3, $2, true);
		SET_LINE_NO($$);
	}
	| TLPAREN nestable_expression TRPAREN
	{
		$$ = $2;
		SET_LINE_NO($$);
	}
	| TLBRAKT nestable_expression TRBRAKT
	{
		Ink_ExpressionList exp_list = Ink_ExpressionList();
		exp_list.push_back($2);
		$$ = new Ink_FunctionExpression(Ink_ParamList(), exp_list, true);
		SET_LINE_NO($$);
	}
	;

%%