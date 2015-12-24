%{
    #include <stdio.h>
    #include <stdlib.h>
	#include <string.h>
	#include "core/expression.h"
	#include "core/error.h"
	#include "interface/engine.h"
	#define SET_LINE_NO(exp) (exp->line_number = current_line_number)
	#define YYERROR_VERBOSE 1
	#define YYDEBUG 1

	extern int current_line_number;
	extern int inkerr_current_line_number;
	const char *yyerror_prefix = "";

	extern int yylex();
	void yyerror(const char *msg) {
		fprintf(stderr, "%sline %d: %s\n", yyerror_prefix, current_line_number, msg);
	}
%}

%union {
	Ink_Expression *expression;
	Ink_ParamList *parameter;
	Ink_ExpressionList *expression_list;
	std::string *string;
	IDContextType context_type;
	InterruptSignal signal;
	int token;
}

%token <string> TIDENTIFIER TNUMERIC TSTRING

%token <token> TVAR TGLOBAL TLET TRETURN TNEW TCLONE
			   TFUNC TINLINE TDO TEND TGO TYIELD TGEN
			   TIMPORT TBREAK TCONTINUE
%token <token> TECLI TDNOT TNOT TCOMMA TSEMICOLON TCOLON TASSIGN
%token <token> TDADD TDSUB TOR TADD TSUB TMUL TDIV TMOD TDOT TNL TLAND TAT
%token <token> TLPAREN TRPAREN TLBRAKT TRBRAKT TLBRACE TRBRACE
%token <token> TARR TINS TOUT
%token <token> TCLE TCLT TCGE TCGT TCEQ TCNE TCAND TCOR

%type <expression> expression assignment_expression
				   primary_expression postfix_expression
				   function_expression additive_expression
				   interrupt_expression multiplicative_expression
				   unary_expression nestable_expression
				   insert_expression field_expression
				   table_expression functional_block
				   block equality_expression
				   relational_expression logical_and_expression
				   logical_or_expression comma_expression
				   import_expression
%type <parameter> param_list param_opt param_list_sub
%type <expression_list> expression_list expression_list_opt
						argument_list argument_list_opt
						element_list element_list_opt
						argument_attachment argument_list_without_paren
%type <context_type> id_context_type
%type <signal> interrupt_signal

%start compile_unit

%%

compile_unit
	: expression_list_opt
	{
		Ink_getCurrentEngine()->top_level = *$1;
		delete $1;
	}
	;

nll
	: TNL
	| nll TNL
	;

nllo
	: /* empty */
	| nll
	;

split
	: nll
	| TSEMICOLON nllo

split_opt
	: /* empty */
	| split

expression_list
	: expression
	{
		$$ = new Ink_ExpressionList();
		$$->push_back($1);
	}
	| expression_list split expression
	{
		$1->push_back($3);
		$$ = $1;
	}
	;

expression_list_opt
	: split_opt
	{
		$$ = new Ink_ExpressionList();
	}
	| split_opt expression_list split_opt
	{
		$$ = $2;
	}
	;

expression
	: nestable_expression
	| comma_expression
	| interrupt_expression
	| import_expression
	;

interrupt_signal
	: TRETURN
	{
		$$ = INTER_RETURN;
	}
	| TBREAK
	{
		$$ = INTER_BREAK;
	}
	| TCONTINUE
	{
		$$ = INTER_CONTINUE;
	}
	;

interrupt_expression
	: interrupt_signal
	{
		$$ = new Ink_InterruptExpression($1, NULL);
		SET_LINE_NO($$);
	}
	| interrupt_signal nestable_expression
	{
		$$ = new Ink_InterruptExpression($1, $2);
		SET_LINE_NO($$);
	}
	;

import_expression
	: TIMPORT nllo argument_list_without_paren
	{
		$$ = new Ink_CallExpression(new Ink_IdentifierExpression(new string("import")), *$3);
		delete $3;
		SET_LINE_NO($$);
	}
	;

comma_expression
	: nestable_expression TCOMMA nllo nestable_expression
	{
		Ink_CommaExpression *tmp = new Ink_CommaExpression();
		tmp->exp_list.push_back($1);
		tmp->exp_list.push_back($4);
		$$ = tmp;
		SET_LINE_NO($$);
	}
	| comma_expression TCOMMA nllo nestable_expression
	{
		as<Ink_CommaExpression>($1)->exp_list.push_back($4);
		$$ = $1;
		SET_LINE_NO($$);
	}
	;

nestable_expression
	: field_expression
	;

field_expression
	: insert_expression
	| TIDENTIFIER TCOLON nllo field_expression
	{
		$$ = new Ink_AssignmentExpression(
				 new Ink_HashExpression(
				 	 new Ink_IdentifierExpression(new string("this")),
				 	 $1),
				 $4);
		SET_LINE_NO($$);
	}

insert_expression
	: logical_or_expression
	| insert_expression TINS nllo logical_or_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("<<")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| insert_expression TOUT nllo logical_or_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string(">>")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression TCOR nllo logical_and_expression
	{
		$$ = new Ink_LogicExpression($1, $4, LOGIC_OR);
		SET_LINE_NO($$);
	}
	;

logical_and_expression
	: assignment_expression
	| logical_and_expression TCAND nllo assignment_expression
	{
		$$ = new Ink_LogicExpression($1, $4, LOGIC_AND);
		SET_LINE_NO($$);
	}
	;

assignment_expression
	: equality_expression
	| equality_expression TASSIGN nllo assignment_expression
	{
		$$ = new Ink_AssignmentExpression($1, $4);
		SET_LINE_NO($$);
	}
	| equality_expression TARR nllo assignment_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("->")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	;

equality_expression
	: relational_expression
	| equality_expression TCEQ nllo relational_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("==")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| equality_expression TCNE nllo relational_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("!=")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	;

relational_expression
	: additive_expression
	| relational_expression TCLT nllo additive_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("<")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| relational_expression TCGT nllo additive_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string(">")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| relational_expression TCLE nllo additive_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("<=")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| relational_expression TCGE nllo additive_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string(">=")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	;

additive_expression
	: multiplicative_expression
	| additive_expression TADD nllo multiplicative_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("+")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| additive_expression TSUB nllo multiplicative_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("-")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression TMUL nllo unary_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("*")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| multiplicative_expression TDIV nllo unary_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("/")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| multiplicative_expression TMOD nllo unary_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back($4);
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("%")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	;

argument_list
	: nestable_expression
	{
		$$ = new Ink_ExpressionList();
		$$->push_back($1);
	}
	| argument_list nllo TCOMMA nllo nestable_expression
	{
		$1->push_back($5);
		$$ = $1;
	}
	;

argument_list_without_paren
	: nestable_expression
	{
		$$ = new Ink_ExpressionList();
		$$->push_back($1);
	}
	| argument_list_without_paren TCOMMA nllo nestable_expression
	{
		$1->push_back($4);
		$$ = $1;
	}
	;

argument_list_opt
	: nllo
	{
		$$ = new Ink_ExpressionList();
	}
	| nllo argument_list nllo
	{
		$$ = $2;
	}
	;

block
	: TLBRACE expression_list_opt TRBRACE
	{
		$$ = new Ink_FunctionExpression(Ink_ParamList(), *$2, true);
		delete $2;
		SET_LINE_NO($$);
	}
	| TDO expression_list_opt TEND
	{
		$$ = new Ink_FunctionExpression(Ink_ParamList(), *$2, true);
		delete $2;
		SET_LINE_NO($$);
	}
	| functional_block
	;

functional_block
	: TLBRACE nllo TOR param_opt TOR expression_list_opt TRBRACE
	{
		$$ = new Ink_FunctionExpression(*$4, *$6, true);
		delete $4;
		delete $6;
		SET_LINE_NO($$);
	}
	| TDO nllo TOR param_opt TOR expression_list_opt TEND
	{
		$$ = new Ink_FunctionExpression(*$4, *$6, true);
		delete $4;
		delete $6;
		SET_LINE_NO($$);
	}
	;

argument_attachment
	: block
	{
		$$ = new Ink_ExpressionList();
		$$->push_back($1);
	}
	| TIDENTIFIER
	{
		$$ = new Ink_ExpressionList();
		$$->push_back(new Ink_StringConstant($1));
	}
	| argument_attachment block
	{
		$1->push_back($2);
		$$ = $1;
	}
	| argument_attachment TIDENTIFIER
	{
		$1->push_back(new Ink_StringConstant($2));
		$$ = $1;
	}
	;

unary_expression
	: table_expression
	| TNEW nllo postfix_expression TLPAREN argument_list_opt TRPAREN
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("new")),
									*$5);
		delete $5;
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TNEW nllo postfix_expression TLPAREN argument_list_opt TRPAREN argument_attachment
	{
		$5->insert($5->end(), $7->begin(), $7->end());
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("new")),
									*$5);
		delete $5;
		delete $7;
		SET_LINE_NO($$);
	}
	| TCLONE nllo unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("clone")),
									Ink_ExpressionList());
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TADD nllo unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("+u")),
									Ink_ExpressionList());
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TSUB nllo unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("-u")),
									Ink_ExpressionList());
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TDNOT nllo unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("!!")),
									Ink_ExpressionList());
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TNOT nllo unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("!")),
									Ink_ExpressionList());
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TDADD nllo unary_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back(new Ink_NumericConstant(1));
		$$ = new Ink_AssignmentExpression($3, new Ink_CallExpression(
												  new Ink_HashExpression($3, new string("+")),
												  arg), false, false);
		SET_LINE_NO($$);
	}
	| TDSUB nllo unary_expression
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back(new Ink_NumericConstant(1));
		$$ = new Ink_AssignmentExpression($3, new Ink_CallExpression(
												  new Ink_HashExpression($3, new string("-")),
												  arg), false, false);
		SET_LINE_NO($$);
	}
	;

element_list
	: nestable_expression
	{
		$$ = new Ink_ExpressionList();
		$$->push_back($1);
	}
	| element_list nllo TCOMMA nllo nestable_expression
	{
		$1->push_back($5);
		$$ = $1;
	}
	;

element_list_opt
	: nllo
	{
		$$ = new Ink_ExpressionList();
	}
	| nllo element_list nllo
	{
		$$ = $2;
	}

table_expression
	: postfix_expression
	| TLBRAKT element_list_opt TRBRAKT
	{
		$$ = new Ink_TableExpression(*$2);
		delete $2;
		SET_LINE_NO($$);
	}
	;

postfix_expression
	: function_expression
	| postfix_expression TDOT nllo TIDENTIFIER
	{
		$$ = new Ink_HashExpression($1, $4);
		SET_LINE_NO($$);
	}
	| postfix_expression TLPAREN argument_list_opt TRPAREN
	{
		$$ = new Ink_CallExpression($1, *$3);
		delete $3;
		SET_LINE_NO($$);
	}
	| postfix_expression TLPAREN argument_list_opt TRPAREN argument_attachment
	{
		$3->insert($3->end(), $5->begin(), $5->end());
		$$ = new Ink_CallExpression($1, *$3);
		delete $3;
		delete $5;
		SET_LINE_NO($$);
	}
	| postfix_expression TLBRAKT nllo argument_list nllo TRBRAKT
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("[]")), *$4);
		delete $4;
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| postfix_expression TDADD
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back(new Ink_NumericConstant(1));
		$$ = new Ink_AssignmentExpression($1, new Ink_CallExpression(
												  new Ink_HashExpression($1, new string("+")),
												  arg), true, false);
		SET_LINE_NO($$);
	}
	| postfix_expression TDSUB
	{
		Ink_ExpressionList arg = Ink_ExpressionList();
		arg.push_back(new Ink_NumericConstant(1));
		$$ = new Ink_AssignmentExpression($1, new Ink_CallExpression(
												  new Ink_HashExpression($1, new string("-")),
												  arg), true, false);
		SET_LINE_NO($$);
	}
	;

param_multi_decl
	: TMUL nllo TLAND nllo
	| TLAND nllo TMUL nllo

param_list_sub
	: TIDENTIFIER
	{
		$$ = new Ink_ParamList();
		$$->push_back(Ink_Parameter($1, false, false));
	}
	| TMUL nllo TIDENTIFIER
	{
		$$ = new Ink_ParamList();
		$$->push_back(Ink_Parameter($3, false, false, true));
	}
	| TLAND nllo TIDENTIFIER
	{
		$$ = new Ink_ParamList();
		$$->push_back(Ink_Parameter($3, true, false));
	}
	| param_multi_decl TIDENTIFIER
	{
		$$ = new Ink_ParamList();
		$$->push_back(Ink_Parameter($2, true, false, true));
	}
	| param_list TCOMMA nllo TIDENTIFIER
	{
		$1->push_back(Ink_Parameter($4, false, false));
		$$ = $1;
	}
	| param_list TCOMMA nllo TMUL nllo TIDENTIFIER
	{
		$1->push_back(Ink_Parameter($6, false, false, true));
		$$ = $1;
	}
	| param_list TCOMMA nllo TLAND nllo TIDENTIFIER
	{
		$1->push_back(Ink_Parameter($6, true, false));
		$$ = $1;
	}
	| param_list TCOMMA nllo param_multi_decl TIDENTIFIER
	{
		$1->push_back(Ink_Parameter($5, true, false, true));
		$$ = $1;
	}
	| param_list TCOMMA nllo TIDENTIFIER TECLI
	{
		$1->push_back(Ink_Parameter($4, false, true));
		$$ = $1;
	}
	| param_list TCOMMA nllo TECLI
	{
		$1->push_back(Ink_Parameter(new string("arg"), false, true));
		$$ = $1;
	}
	;

param_list
	: param_list_sub
	| TIDENTIFIER TECLI
	{
		$$ = new Ink_ParamList();
		$$->push_back(Ink_Parameter($1, false, true));
	}
	| TECLI
	{
		$$ = new Ink_ParamList();
		$$->push_back(Ink_Parameter(new string("arg"), false, true));
	}
	;

param_opt
	: nllo
	{
		$$ = new Ink_ParamList();
	}
	| nllo param_list nllo
	{
		$$ = $2;
	}
	;

function_expression
	: primary_expression
	| TFUNC nllo TLPAREN param_opt TRPAREN nllo TLBRACE expression_list_opt TRBRACE
	{
		$$ = new Ink_FunctionExpression(*$4, *$8);
		delete $4;
		delete $8;
		SET_LINE_NO($$);
	}
	| TFUNC nllo TLPAREN param_opt TRPAREN nllo TDO expression_list_opt TEND
	{
		$$ = new Ink_FunctionExpression(*$4, *$8);
		delete $4;
		delete $8;
		SET_LINE_NO($$);
	}
	| TINLINE nllo TLPAREN param_opt TRPAREN nllo TLBRACE expression_list_opt TRBRACE
	{
		$$ = new Ink_FunctionExpression(*$4, *$8, true);
		delete $4;
		delete $8;
		SET_LINE_NO($$);
	}
	| TINLINE nllo TLPAREN param_opt TRPAREN nllo TDO expression_list_opt TEND
	{
		$$ = new Ink_FunctionExpression(*$4, *$8, true);
		delete $4;
		delete $8;
		SET_LINE_NO($$);
	}
	| TGEN nllo TLPAREN param_opt TRPAREN nllo TLBRACE expression_list_opt TRBRACE
	{
		$$ = new Ink_FunctionExpression(*$4, *$8, false, true);
		delete $4;
		delete $8;
		SET_LINE_NO($$);
	}
	| TGEN nllo TLPAREN param_opt TRPAREN nllo TDO expression_list_opt TEND
	{
		$$ = new Ink_FunctionExpression(*$4, *$8, false, true);
		delete $4;
		delete $8;
		SET_LINE_NO($$);
	}
	| functional_block
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
	: TNUMERIC
	{
		// printf("numeric: %s\n", $1->c_str());
		$$ = Ink_NumericConstant::parse(*$1);
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
	| TVAR nllo TIDENTIFIER
	{
		$$ = new Ink_IdentifierExpression($3, ID_COMMON, true);
		SET_LINE_NO($$);
	}
	| id_context_type nllo TIDENTIFIER
	{
		$$ = new Ink_IdentifierExpression($3, $1);
		SET_LINE_NO($$);
	}
	| TVAR nllo id_context_type nllo TIDENTIFIER
	{
		$$ = new Ink_IdentifierExpression($5, $3, true);
		SET_LINE_NO($$);
	}
	| id_context_type nllo TVAR nllo TIDENTIFIER
	{
		$$ = new Ink_IdentifierExpression($5, $1, true);
		SET_LINE_NO($$);
	}
	| TLPAREN nestable_expression TRPAREN
	{
		$$ = $2;
		SET_LINE_NO($$);
	}
	;

%%