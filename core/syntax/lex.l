
SP				[ \t\v\f]
NEWLINE			[\r\n]
PM				[+-]

BIN_PRE			(0[bB])
HEX_PRE			(0[xX])
OCT_PRE			(0[oO])

BIN				[01]
DEC				[0-9]
HEX				[a-fA-F0-9]
OCT				[0-7]
POWER_MARK		[eE]
LETTER			[a-zA-Z_?$]
FIRST_LETTER	({LETTER}|[@])
EXP_OPT			(({POWER_MARK}[-+]?{DEC}+)?)

DIGITS			({DIGIT})+
SPS				({SP})+

%{
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "syntax.h"
#include "core/expression.h"
#include "core/interface/engine.h"
#include "includes/universal.h"
#include "grammar.hpp"

#undef YY_INPUT
#define YY_INPUT(buf, result, max_size) (result = ink_yyinput(buf, max_size))
#define SAVE_TOKEN()		(yylval.string = new std::string(yytext, yyleng))
#define TOKEN(t)			(yylval.token = t)
#define LINE_NUMBER_INC()	(current_line_number++)
#define MIN(a, b) ((unsigned int)(a) < (unsigned int)(b) ? (a) : (b))

using namespace ink;

static Ink_LineNoType current_line_number = 1;
static const char *yyerror_prefix = "";

namespace ink {

void InkParser_setParserInfo(Ink_LineNoType lineno, const char *yyprefix)
{
	InkParser_lockParseLock();
	current_line_number = lineno;
	if (yyerror_prefix)
		yyerror_prefix = yyprefix;
	InkParser_unlockParseLock();
	return;
}

Ink_LineNoType InkParser_getCurrentLineno()
{
	return current_line_number;
}

const char *InkParser_getErrPrefix()
{
	return yyerror_prefix;
}

}

static string *id_literal = NULL;
static string *string_literal = NULL;
static bool if_ignore_nl = false;

int file_input(char *buf, int max_size)
{
	int ch;
	int len;

	if (feof(yyin))
		return 0;

	for (len = 0; len < max_size; len++) {
		ch = getc(yyin);
		if (ch == EOF)
			break;
		buf[len] = ch;
	}
	return len;
}

static const char **ink_source_string;
static int ink_current_source_line;
static int ink_current_char_index;

namespace ink {
void Ink_setStringInput(const char **source)
{
	ink_source_string = source;
	ink_current_source_line = 0;
	ink_current_char_index = 0;
}
}

int string_input(char *buf, int max_size)
{
	int len;

	if (ink_source_string[ink_current_source_line] == NULL)
		return 0;

	while (ink_source_string[ink_current_source_line][ink_current_char_index] == '\0') {
		ink_current_source_line++;
		ink_current_char_index = 0;
		if (ink_source_string[ink_current_source_line] == NULL)
			return 0;
	}

	if (ink_source_string[ink_current_source_line] == NULL)
		return 0;

	len = MIN(strlen(ink_source_string[ink_current_source_line]) - ink_current_char_index,
			  max_size);
	strncpy(buf, &ink_source_string[ink_current_source_line][ink_current_char_index], len);
	ink_current_char_index += len;

	return len;
}

int ink_yyinput(char *buf, int max_size)
{
	int result;

	switch (InkParser_getParseEngine()->input_mode) {
		case INK_STRING_INPUT:
			result = string_input(buf, max_size);
			break;
		case INK_FILE_INPUT:
		default:
			result = file_input(buf, max_size);
			break;
	}

	return result;
}

void yyerror(const char *msg);

%}

%option noyywrap
%start C_COMMENT CC_COMMENT ID_LITERAL DOUBLE_STRING_LITERAL SINGLE_STRING_LITERAL

%%

<INITIAL>"new"						return TOKEN(TNEW);
<INITIAL>"delete"					return TOKEN(TDELETE);
<INITIAL>"clone"					return TOKEN(TCLONE);
<INITIAL>"fix"						return TOKEN(TFIX);

<INITIAL>"fn"						return TOKEN(TFUNC);
<INITIAL>"inl"						return TOKEN(TINLINE);
<INITIAL>"macro"					return TOKEN(TMACRO);

<INITIAL>"do"						return TOKEN(TDO);
<INITIAL>"end"						return TOKEN(TEND);
<INITIAL>"import"					return TOKEN(TIMPORT);

<INITIAL>"retn"						return TOKEN(TRETURN);
<INITIAL>"break"					return TOKEN(TBREAK);
<INITIAL>"continue"					return TOKEN(TCONTINUE);
<INITIAL>"drop"						return TOKEN(TDROP);
<INITIAL>"throw"					return TOKEN(TTHROW);
<INITIAL>"retry"					return TOKEN(TRETRY);
<INITIAL>"exit"						return TOKEN(TEXIT);

<INITIAL>"yield"					return TOKEN(TYIELD);
<INITIAL>"with"						return TOKEN(TWITH);

<INITIAL>"&&"						return TOKEN(TLAND);
<INITIAL>"||"						return TOKEN(TLOR);

<INITIAL>">>="						return(TAOUT);
<INITIAL>"<<="						return(TAINS);
<INITIAL>"+="						return(TAADD);
<INITIAL>"-="						return(TASUB);
<INITIAL>"*="						return(TAMUL);
<INITIAL>"/="						return(TADIV);
<INITIAL>"%="						return(TAMOD);
<INITIAL>"&="						return(TAAND);
<INITIAL>"^="						return(TAXOR);
<INITIAL>"|="						return(TAOR);
<INITIAL>"&&="						return(TALAND);
<INITIAL>"||="						return(TALOR);

<INITIAL>"&"						return TOKEN(TBAND);
<INITIAL>"~"						return TOKEN(TBINV);
<INITIAL>"^"						return TOKEN(TBXOR);
<INITIAL>"|"						return TOKEN(TBOR);
<INITIAL>"<<"						return TOKEN(TINS);
<INITIAL>">>"						return TOKEN(TOUT);

<INITIAL>"..."						return TOKEN(TECLI);
<INITIAL>"!!"						return TOKEN(TDNOT);
<INITIAL>"!="						return TOKEN(TCNE);
<INITIAL>"!"						return TOKEN(TLNOT);
<INITIAL>","						return TOKEN(TCOMMA);
<INITIAL>";"						return TOKEN(TSEMICOLON);
<INITIAL>"::"						return TOKEN(TDCOLON);
<INITIAL>":"						return TOKEN(TCOLON);
<INITIAL>"."						return TOKEN(TDOT);

<INITIAL>"->"						return TOKEN(TARR);
<INITIAL>"<=>"						return TOKEN(TCSS);
<INITIAL>"<="						return TOKEN(TCLE);
<INITIAL>"<"						return TOKEN(TCLT);
<INITIAL>">="						return TOKEN(TCGE);
<INITIAL>">"						return TOKEN(TCGT);
<INITIAL>"=="						return TOKEN(TCEQ);
<INITIAL>"="						return TOKEN(TASSIGN);

<INITIAL>"("						return TOKEN(TLPAREN);
<INITIAL>")"						return TOKEN(TRPAREN);
<INITIAL>"["						return TOKEN(TLBRAKT);
<INITIAL>"]"						return TOKEN(TRBRAKT);
<INITIAL>"{"						return TOKEN(TLBRACE);
<INITIAL>"}"						return TOKEN(TRBRACE);

<INITIAL>"++"						return TOKEN(TDADD);
<INITIAL>"--"						return TOKEN(TDSUB);
<INITIAL>"+"						return TOKEN(TADD);
<INITIAL>"-"						return TOKEN(TSUB);
<INITIAL>"*"						return TOKEN(TMUL);
<INITIAL>"/"						return TOKEN(TDIV);
<INITIAL>"%"						return TOKEN(TMOD);

<INITIAL>"\\"{SP}*{NEWLINE} {
	LINE_NUMBER_INC();
}
<INITIAL>{NEWLINE} {
	LINE_NUMBER_INC();
	if (!if_ignore_nl)
		return TOKEN(TNL);
}
<INITIAL>{SP}						/* Blank */;

 /* Constants */
<INITIAL>{FIRST_LETTER}({LETTER}|{DEC})* {
	SAVE_TOKEN();
	if (InkParser_getParseEngine()->findProtocol(yylval.string->c_str())) {
		return TPROTOCOL;
	}
	return TIDENTIFIER;
}
<INITIAL>{BIN_PRE}{BIN}+\.{BIN}+{EXP_OPT} { // bin
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>{BIN_PRE}{BIN}+{EXP_OPT} { // bin
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>{OCT_PRE}{OCT}+\.{OCT}+{EXP_OPT} { // oct
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>{OCT_PRE}{OCT}+{EXP_OPT} { // oct
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>{HEX_PRE}{HEX}+\.{HEX}+{EXP_OPT} { // hex
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>{HEX_PRE}{HEX}+{EXP_OPT} { // hex
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>{DEC}*\.{DEC}+{EXP_OPT} { // dec
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>0{EXP_OPT} { // dec
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>[1-9]{DEC}*{EXP_OPT} { // dec
	SAVE_TOKEN();
	return TNUMERIC;
}

 /* Comments */
<INITIAL>"/*"     							BEGIN C_COMMENT;
<INITIAL>"//"     							BEGIN CC_COMMENT;
<INITIAL>"#"     							BEGIN CC_COMMENT;

<C_COMMENT>{NEWLINE} {
	LINE_NUMBER_INC();
}
<C_COMMENT>"*/"								BEGIN INITIAL;
<C_COMMENT><<EOF>> {
	yyerror("EOF in comment");
	exit(1);
}
<C_COMMENT>.								;
<CC_COMMENT>{NEWLINE} {
	LINE_NUMBER_INC();
	BEGIN INITIAL;
	return TOKEN(TNL);
}
<CC_COMMENT><<EOF>>							BEGIN INITIAL;
<CC_COMMENT>.								;

 /* ID Literal */
<INITIAL>` {
	id_literal = new std::string("", 0);
    BEGIN ID_LITERAL;
}
<ID_LITERAL>` {
	yylval.string = id_literal;
	id_literal = NULL;
	BEGIN INITIAL;
	return TIDENTIFIER;
}
<ID_LITERAL>\\{OCT}{1,3} {
	unsigned int letter;
	sscanf(&yytext[1], "%o", &letter);
    *id_literal += letter;
}
<ID_LITERAL>\\[xX]{HEX}{1,2} {
	unsigned int letter;
	sscanf(&yytext[2], "%x", &letter);
    *id_literal += letter;
}
<ID_LITERAL>{NEWLINE}        {
	*id_literal += yytext[0];
    LINE_NUMBER_INC();
}
<ID_LITERAL>\\n       *id_literal += '\n';
<ID_LITERAL>\\t       *id_literal += '\t';
<ID_LITERAL>\\\\      *id_literal += '\\';
<ID_LITERAL><<EOF>>   {
	yyerror("EOF in id literal");
	yyterminate();
}
<ID_LITERAL>.         {
    *id_literal += yytext[0];
}

 /* Double String Literal */
<INITIAL>\" {
	string_literal = new std::string("", 0);
    BEGIN DOUBLE_STRING_LITERAL;
}
<DOUBLE_STRING_LITERAL>\" {
	yylval.string = string_literal;
	string_literal = NULL;
	BEGIN INITIAL;
	return TSTRING;
}
<DOUBLE_STRING_LITERAL>\\{OCT}{1,3} {
	unsigned int letter;
	sscanf(&yytext[1], "%o", &letter);
    *string_literal += letter;
}
<DOUBLE_STRING_LITERAL>\\[xX]{HEX}{1,2} {
	unsigned int letter;
	sscanf(&yytext[2], "%x", &letter);
    *string_literal += letter;
}
<DOUBLE_STRING_LITERAL>{NEWLINE}        {
	*string_literal += yytext[0];
    LINE_NUMBER_INC();
}
<DOUBLE_STRING_LITERAL>\\\"      *string_literal += '"';
<DOUBLE_STRING_LITERAL>\\n       *string_literal += '\n';
<DOUBLE_STRING_LITERAL>\\t       *string_literal += '\t';
<DOUBLE_STRING_LITERAL>\\\\      *string_literal += '\\';
<DOUBLE_STRING_LITERAL><<EOF>>   {
	yyerror("EOF in string literal");
	yyterminate();
}
<DOUBLE_STRING_LITERAL>.         {
    *string_literal += yytext[0];
}

 /* Single String Literal */
<INITIAL>' {
	string_literal = new std::string("", 0);
    BEGIN SINGLE_STRING_LITERAL;
}
<SINGLE_STRING_LITERAL>' {
	yylval.string = string_literal;
	string_literal = NULL;
	BEGIN INITIAL;
	return TSTRING;
}
<SINGLE_STRING_LITERAL>\\{OCT}{1,3} {
	unsigned int letter;
	sscanf(&yytext[1], "%o", &letter);
    *string_literal += letter;
}
<SINGLE_STRING_LITERAL>\\[xX]{HEX}{1,2} {
	unsigned int letter;
	sscanf(&yytext[2], "%x", &letter);
    *string_literal += letter;
}
<SINGLE_STRING_LITERAL>{NEWLINE}        {
	*string_literal += yytext[0];
    LINE_NUMBER_INC();
};
<SINGLE_STRING_LITERAL>\\'       *string_literal += '\'';
<SINGLE_STRING_LITERAL>\\n       *string_literal += '\n';
<SINGLE_STRING_LITERAL>\\t       *string_literal += '\t';
<SINGLE_STRING_LITERAL>\\\\      *string_literal += '\\';
<SINGLE_STRING_LITERAL><<EOF>>   {
	yyerror("EOF in string literal");
	yyterminate();
}
<SINGLE_STRING_LITERAL>.         {
    *string_literal += yytext[0];
}

. {
	char buf[30];
	sprintf(buf, "Unknown token \\x%x", yytext[0]);
	yyerror(buf);
	yyterminate();
}

%%
