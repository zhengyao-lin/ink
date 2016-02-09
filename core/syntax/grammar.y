%{
    #include <stdio.h>
    #include <stdlib.h>
	#include <string.h>
	#include "syntax.h"
	#include "core/expression.h"
	#include "core/error.h"
	#include "core/general.h"
	#include "core/interface/engine.h"
	#include "core/interface/setting.h"
	#define SET_LINE_NO(exp) (exp->line_number = InkParser_getCurrentLineno())
	#define YYERROR_VERBOSE 1
	#define YYDEBUG 1

	using namespace ink;

	extern int yylex();
	void yyerror(const char *msg) {
		const char *tmp = InkParser_getParseEngine()->getFilePath();
		fprintf(stderr, "%s: %sline %ld: %s\n", tmp ? tmp : "<unknown input>",
				InkParser_getErrPrefix(), InkParser_getCurrentLineno(), msg);
	}

%}

%union {
	ink::Ink_Expression					*expression;
	ink::Ink_ParamList					*parameter;
	ink::Ink_ExpressionList				*expression_list;
	ink::Ink_Argument					*argument;
	ink::Ink_ArgumentList				*argument_list;
	ink::Ink_HashTableMapping			*hash_table_mapping;
	ink::Ink_HashTableMappingSingle		*hash_table_mapping_single;
	std::string							*string;
	ink::Ink_InterruptSignal			signal;
	int									split;
	int									token;
}

%token <string> TIDENTIFIER TNUMERIC TSTRING TPROTOCOL TUNKNOWN

%token <token> TRETURN TNEW TDELETE TCLONE
			   TFUNC TINLINE TMACRO TDO TEND
			   TIMPORT TBREAK TCONTINUE TDROP TTHROW TRETRY TEXIT
			   TYIELD TWITH
%token <token> TECLI TDNOT TNOT TCOMMA TSEMICOLON TDCOLON TCOLON TASSIGN
%token <token> TDADD TDSUB TOR TADD TSUB TMUL TDIV TMOD TDOT TNL TLAND
%token <token> TLPAREN TRPAREN TLBRAKT TRBRAKT TLBRACE TRBRACE
%token <token> TARR TINS TOUT
%token <token> TCLE TCLT TCGE TCGT TCEQ TCNE TCAND TCOR

%type <expression> expression assignment_expression
				   primary_expression postfix_expression
				   function_expression additive_expression
				   interrupt_expression multiplicative_expression
				   unary_expression nestable_expression
				   insert_expression field_expression
				   functional_block import_expression
				   block equality_expression
				   relational_expression logical_and_expression
				   logical_or_expression comma_expression
				   single_element_expression space_hash_expression
				   dot_hash_expression direct_argument_attachment_expression
%type <parameter> param_list param_opt param_list_sub
%type <expression_list> top_level_expression_list top_level_expression_list_opt
						expression_list expression_list_opt
						element_list element_list_opt
%type <argument> direct_argument_attachment_expression_prefix
%type <argument_list> argument_list argument_list_opt
					  argument_attachment argument_attachment_opt
					  argument_list_without_paren insert_list
%type <hash_table_mapping> hash_table_mapping hash_table_mapping_opt
%type <hash_table_mapping_single> hash_table_mapping_single
%type <split> split split_opt
%type <signal> interrupt_signal

%start compile_unit

%%

compile_unit
	: top_level_expression_list_opt
	{
		InkParser_getParseEngine()->top_level = *$1;
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
	{
		$$ = -1;
	}
	| TSEMICOLON nll
	{
		$$ = 0;
	}
	| TSEMICOLON
	{
		$$ = 0;
	}
	;

split_opt
	: /* empty */
	{
		$$ = 0;
	}
	| split
	;

top_level_expression_list
	: expression
	{
		$$ = new Ink_ExpressionList();
		$$->push_back($1);
		if (InkParser_getParseEngine()->getFilePath()) {
			$1->file_name
			= ($1->file_name_p
			   = new string(InkParser_getParseEngine()->getFilePath()))->c_str();
		}
	}
	| top_level_expression_list split expression
	{
		(*$1)[$1->size() - 1]->line_number += $2;
		$1->push_back($3);
		$$ = $1;
		$3->file_name = $$->front()->file_name;
	}
	;

top_level_expression_list_opt
	: split_opt
	{
		$$ = new Ink_ExpressionList();
	}
	| split_opt top_level_expression_list split_opt
	{
		$$ = $2;
	}
	;

expression_list
	: expression
	{
		$$ = new Ink_ExpressionList();
		$$->push_back($1);
		if (InkParser_getParseEngine()->getFilePath()) {
			$1->file_name
			= ($1->file_name_p
			   = new string(InkParser_getParseEngine()->getFilePath()))->c_str();
		}
	}
	| expression_list split expression
	{
		(*$1)[$1->size() - 1]->line_number += $2;
		$1->push_back($3);
		$$ = $1;
		$3->file_name = $$->front()->file_name;
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
	| TDROP
	{
		$$ = INTER_DROP;
	}
	| TTHROW
	{
		$$ = INTER_THROW;
	}
	| TRETRY
	{
		$$ = INTER_RETRY;
	}
	| TEXIT
	{
		$$ = INTER_EXIT;
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
	| TCOLON TIDENTIFIER
	{
		$$ = new Ink_InterruptExpression($2, NULL);
		SET_LINE_NO($$);
	}
	| TCOLON TIDENTIFIER nestable_expression
	{
		$$ = new Ink_InterruptExpression($2, $3);
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
	;

insert_list
	: TINS nllo logical_or_expression
	{
		$$ = new Ink_ArgumentList();
		$$->push_back(new Ink_Argument($3));
	}
	| insert_list TINS nllo logical_or_expression
	{
		$1->push_back(new Ink_Argument($4));
		$$ = $1;
	}
	;

insert_expression
	: logical_or_expression
	| logical_or_expression insert_list
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("<<")), *$2);
		delete $2;
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| insert_expression TOUT nllo logical_or_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));

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
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("->")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	;

equality_expression
	: relational_expression
	| equality_expression TCEQ nllo relational_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("==")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| equality_expression TCNE nllo relational_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("!=")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	;

relational_expression
	: additive_expression
	| relational_expression TCLT nllo additive_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("<")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| relational_expression TCGT nllo additive_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string(">")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| relational_expression TCLE nllo additive_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("<=")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| relational_expression TCGE nllo additive_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));

		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string(">=")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	;

additive_expression
	: multiplicative_expression
	| additive_expression TADD nllo multiplicative_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("+")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| additive_expression TSUB nllo multiplicative_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("-")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression TMUL nllo unary_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("*")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| multiplicative_expression TDIV nllo unary_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("/")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| multiplicative_expression TMOD nllo unary_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("%")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	;

argument_list
	: nestable_expression
	{
		$$ = new Ink_ArgumentList();
		$$->push_back(new Ink_Argument($1));
	}
	| argument_list nllo TCOMMA nllo nestable_expression
	{
		$1->push_back(new Ink_Argument($5));
		$$ = $1;
	}
	;

argument_list_without_paren
	: nestable_expression
	{
		$$ = new Ink_ArgumentList();
		$$->push_back(new Ink_Argument($1));
	}
	| argument_list_without_paren TCOMMA nllo nestable_expression
	{
		$1->push_back(new Ink_Argument($4));
		$$ = $1;
	}
	;

argument_list_opt
	: nllo
	{
		$$ = new Ink_ArgumentList();
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

unary_expression
	: postfix_expression
	| TNEW nllo postfix_expression TLPAREN argument_list_opt TRPAREN
	{
		$$ = new Ink_CallExpression($3, *$5, true);
		delete $5;
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TNEW nllo postfix_expression TLPAREN argument_list_opt TRPAREN argument_attachment
	{
		$5->insert($5->end(), $7->begin(), $7->end());
		$$ = new Ink_CallExpression($3, *$5, true);
		delete $5;
		delete $7;
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TCLONE nllo unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("clone")),
									Ink_ArgumentList());
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TDELETE nllo unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("delete")),
									Ink_ArgumentList());
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TADD nllo unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("+u")),
									Ink_ArgumentList());
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TSUB nllo unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("-u")),
									Ink_ArgumentList());
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TDNOT nllo unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("!!")),
									Ink_ArgumentList());
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TNOT nllo unary_expression
	{
		$$ = new Ink_CallExpression(new Ink_HashExpression($3, new string("!")),
									Ink_ArgumentList());
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
	}
	| TDADD nllo unary_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument(new Ink_NumericConstant(1)));
		$$ = new Ink_AssignmentExpression($3, new Ink_CallExpression(
												  new Ink_HashExpression($3, new string("+")),
												  arg), false, false);
		SET_LINE_NO($$);
	}
	| TDSUB nllo unary_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument(new Ink_NumericConstant(1)));
		$$ = new Ink_AssignmentExpression($3, new Ink_CallExpression(
												  new Ink_HashExpression($3, new string("-")),
												  arg), false, false);
		SET_LINE_NO($$);
	}
	| TYIELD unary_expression
	{
		$$ = new Ink_YieldExpression($2);
		SET_LINE_NO($$);
	}
	;

argument_attachment
	: block
	{
		$$ = new Ink_ArgumentList();
		$$->push_back(new Ink_Argument($1));
	}
	| TIDENTIFIER nllo TLPAREN element_list_opt TRPAREN
	{
		$$ = new Ink_ArgumentList();
		$$->push_back(new Ink_Argument(new Ink_StringConstant($1)));
		$$->push_back(new Ink_Argument(new Ink_TableExpression(*$4)));
		delete $4;
	}
	| TWITH nllo function_expression
	{
		$$ = new Ink_ArgumentList();
		$$->push_back(new Ink_Argument(true, $3));
	}
	| block argument_attachment
	{
		$$ = new Ink_ArgumentList();
		$$->push_back(new Ink_Argument($1));
		$$->insert($$->end(), $2->begin(), $2->end());
		delete $2;
	}
	| TIDENTIFIER nllo argument_attachment
	{
		$$ = new Ink_ArgumentList();
		$$->push_back(new Ink_Argument(new Ink_StringConstant($1)));
		$$->insert($$->end(), $3->begin(), $3->end());
		delete $3;
	}
	| TIDENTIFIER nllo TLPAREN element_list_opt TRPAREN argument_attachment
	{
		$$ = new Ink_ArgumentList();
		$$->push_back(new Ink_Argument(new Ink_StringConstant($1)));
		$$->push_back(new Ink_Argument(new Ink_TableExpression(*$4)));
		$$->insert($$->end(), $6->begin(), $6->end());
		delete $4;
		delete $6;
	}
	| TWITH nllo function_expression argument_attachment
	{
		$$ = new Ink_ArgumentList();
		$$->push_back(new Ink_Argument(true, $3));
		$$->insert($$->end(), $4->begin(), $4->end());
		delete $4;
	}
	;

argument_attachment_opt
	: /* empty */
	{
		$$ = new Ink_ArgumentList();
	}
	| argument_attachment
	;

space_hash_expression
	: primary_expression TIDENTIFIER
	{
		$$ = new Ink_HashExpression($1, $2);
		SET_LINE_NO($$);
	}
	| space_hash_expression TIDENTIFIER
	{
		$$ = new Ink_HashExpression($1, $2);
		SET_LINE_NO($$);
	}
	;

dot_hash_expression
	: postfix_expression TDOT nllo TIDENTIFIER
	{
		$$ = new Ink_HashExpression($1, $4);
		SET_LINE_NO($$);
	}
	;

direct_argument_attachment_expression_prefix
	: block
	{
		$$ = new Ink_Argument($1);
	}
	| TWITH nllo function_expression
	{
		$$ = new Ink_Argument(true, $3);
	}
	;

direct_argument_attachment_expression
	: dot_hash_expression
	| space_hash_expression
	| function_expression
	;

postfix_expression
	: function_expression
	| space_hash_expression
	| dot_hash_expression
	| postfix_expression TLPAREN argument_list_opt TRPAREN argument_attachment_opt
	{
		$3->insert($3->end(), $5->begin(), $5->end());
		$$ = new Ink_CallExpression($1, *$3);
		delete $3;
		delete $5;
		SET_LINE_NO($$);
	}
	| direct_argument_attachment_expression direct_argument_attachment_expression_prefix argument_attachment_opt
	{
		Ink_ArgumentList arg_list = Ink_ArgumentList();
		arg_list.push_back($2);
		arg_list.insert(arg_list.end(), $3->begin(), $3->end());
		$$ = new Ink_CallExpression($1, arg_list);
		delete $3;
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
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument(new Ink_NumericConstant(1)));
		$$ = new Ink_AssignmentExpression($1, new Ink_CallExpression(
												  new Ink_HashExpression($1, new string("+")),
												  arg), true, false);
		SET_LINE_NO($$);
	}
	| postfix_expression TDSUB
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument(new Ink_NumericConstant(1)));
		$$ = new Ink_AssignmentExpression($1, new Ink_CallExpression(
												  new Ink_HashExpression($1, new string("-")),
												  arg), true, false);
		SET_LINE_NO($$);
	}
	| postfix_expression TDCOLON nllo function_expression
	{
		Ink_ArgumentList arg = Ink_ArgumentList();
		arg.push_back(new Ink_Argument($4));
		$$ = new Ink_CallExpression(new Ink_HashExpression($1, new string("::")), arg);
		SET_LINE_NO($$);
		SET_LINE_NO(as<Ink_CallExpression>($$)->callee);
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
	| param_list TCOMMA nllo TLAND nllo TIDENTIFIER TECLI
	{
		$1->push_back(Ink_Parameter($6, true, true));
		$$ = $1;
	}
	| param_list TCOMMA nllo TECLI
	{
		$1->push_back(Ink_Parameter(new string("args"), false, true));
		$$ = $1;
	}
	| param_list TCOMMA nllo TLAND nllo TECLI
	{
		$1->push_back(Ink_Parameter(new string("args"), true, true));
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
	| TLAND nllo TIDENTIFIER TECLI
	{
		$$ = new Ink_ParamList();
		$$->push_back(Ink_Parameter($3, true, true));
	}
	| TECLI
	{
		$$ = new Ink_ParamList();
		$$->push_back(Ink_Parameter(new string("args"), false, true));
	}
	| TLAND nllo TECLI
	{
		$$ = new Ink_ParamList();
		$$->push_back(Ink_Parameter(new string("args"), true, true));
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
	| TMACRO nllo TLPAREN param_opt TRPAREN nllo TLBRACE expression_list_opt TRBRACE
	{
		$$ = new Ink_FunctionExpression(*$4, *$8, false, true);
		delete $4;
		delete $8;
		SET_LINE_NO($$);
	}
	| TMACRO nllo TLPAREN param_opt TRPAREN nllo TDO expression_list_opt TEND
	{
		$$ = new Ink_FunctionExpression(*$4, *$8, false, true);
		delete $4;
		delete $8;
		SET_LINE_NO($$);
	}
	| TPROTOCOL nllo TLPAREN param_opt TRPAREN nllo TLBRACE expression_list_opt TRBRACE
	{
		$$ = new Ink_FunctionExpression(*$4, *$8, $1);
		delete $4;
		delete $8;
		SET_LINE_NO($$);
	}
	| TPROTOCOL nllo TLPAREN param_opt TRPAREN nllo TDO expression_list_opt TEND
	{
		$$ = new Ink_FunctionExpression(*$4, *$8, $1);
		delete $4;
		delete $8;
		SET_LINE_NO($$);
	}
	| functional_block
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
	;

hash_table_mapping_single
	: TIDENTIFIER nllo TCOLON nllo nestable_expression
	{
		$$ = new Ink_HashTableMappingSingle($1, $5);
		SET_LINE_NO($$);
	}
	| TSTRING nllo TCOLON nllo nestable_expression
	{
		$$ = new Ink_HashTableMappingSingle($1, $5);
		SET_LINE_NO($$);
	}
	| TLBRAKT nestable_expression TRBRAKT nllo TCOLON nllo nestable_expression
	{
		$$ = new Ink_HashTableMappingSingle($2, $7);
		SET_LINE_NO($$);
	}
	;

hash_table_mapping
	: hash_table_mapping_single
	{
		$$ = new Ink_HashTableMapping();
		$$->push_back($1);
	}
	| hash_table_mapping nllo TCOMMA nllo hash_table_mapping_single
	{
		$1->push_back($5);
		$$ = $1;
	}
	;

hash_table_mapping_opt
	: nllo
	{
		$$ = new Ink_HashTableMapping();
	}
	| nllo hash_table_mapping nllo
	{
		$$ = $2;
	}
	;

single_element_expression
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
	| TUNKNOWN
	{
		$$ = new Ink_IdentifierExpression($1);
		$$->is_unknown = true;
		SET_LINE_NO($$);
	}
	;

primary_expression
	: single_element_expression
	| TLPAREN nllo nestable_expression nllo TRPAREN
	{
		$$ = $3;
		SET_LINE_NO($$);
	}
	| TLPAREN nllo comma_expression nllo TRPAREN
	{
		$$ = $3;
		SET_LINE_NO($$);
	}
	| TLBRAKT element_list_opt TRBRAKT
	{
		$$ = new Ink_TableExpression(*$2);
		delete $2;
		SET_LINE_NO($$);
	}
	| TLBRACE hash_table_mapping_opt TRBRACE
	{
		$$ = new Ink_HashTableExpression(*$2);
		delete $2;
		SET_LINE_NO($$);
	}
	;

%%