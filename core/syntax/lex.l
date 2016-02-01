
SP				[ \t\v\f]
NEWLINE			[\r\n]
PM				[+-]
DEC				[0-9]
HEX				[a-fA-F0-9]
OCT				[0-7]
LETTER			[a-zA-Z_?$]

DIGITS			({DIGIT})+
SPS				({SP})+

%{
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "includes/universal.h"
#include "core/expression.h"
#include "core/interface/engine.h"
#include "grammar.hpp"

#undef YY_INPUT
#define YY_INPUT(buf, result, max_size) (result = ink_yyinput(buf, max_size))
#define SAVE_TOKEN()		(yylval.string = new std::string(yytext, yyleng))
#define TOKEN(t)			(yylval.token = t)
#define LINE_NUMBER_INC()	(current_line_number++)
#define MIN(a, b) ((unsigned int)(a) < (unsigned int)(b) ? (a) : (b))

using namespace ink;

namespace ink {
Ink_LineNoType current_line_number = 1;
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

	switch (Ink_getParseEngine()->input_mode) {
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

%}

%option noyywrap
%start C_COMMENT CC_COMMENT ID_LITERAL STRING_LITERAL

%%

<INITIAL>"retn"						return TOKEN(TRETURN);
<INITIAL>"new"						return TOKEN(TNEW);
<INITIAL>"delete"					return TOKEN(TDELETE);
<INITIAL>"clone"					return TOKEN(TCLONE);
<INITIAL>"fn"						return TOKEN(TFUNC);
<INITIAL>"inl"						return TOKEN(TINLINE);
<INITIAL>"do"						return TOKEN(TDO);
<INITIAL>"end"						return TOKEN(TEND);
<INITIAL>"import"					return TOKEN(TIMPORT);
<INITIAL>"break"					return TOKEN(TBREAK);
<INITIAL>"continue"					return TOKEN(TCONTINUE);
<INITIAL>"drop"						return TOKEN(TDROP);
<INITIAL>"throw"					return TOKEN(TTHROW);
<INITIAL>"retry"					return TOKEN(TRETRY);
<INITIAL>"exit"						return TOKEN(TEXIT);
<INITIAL>"yield"					return TOKEN(TYIELD);
<INITIAL>"with"						return TOKEN(TWITH);
<INITIAL>"_" {
	SAVE_TOKEN();
	return TUNKNOWN;
}

<INITIAL>"&&"						return TOKEN(TCAND);
<INITIAL>"||"						return TOKEN(TCOR);

<INITIAL>"&"						return TOKEN(TLAND);

<INITIAL>"..."						return TOKEN(TECLI);
<INITIAL>"!!"						return TOKEN(TDNOT);
<INITIAL>"!="						return TOKEN(TCNE);
<INITIAL>"!"						return TOKEN(TNOT);
<INITIAL>","						return TOKEN(TCOMMA);
<INITIAL>";"						return TOKEN(TSEMICOLON);
<INITIAL>"::"						return TOKEN(TDCOLON);
<INITIAL>":"						return TOKEN(TCOLON);
<INITIAL>"."						return TOKEN(TDOT);

<INITIAL>"->"						return TOKEN(TARR);
<INITIAL>"<<"						return TOKEN(TINS);
<INITIAL>">>"						return TOKEN(TOUT);
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
<INITIAL>"|"						return TOKEN(TOR);
<INITIAL>"+"						return TOKEN(TADD);
<INITIAL>"-"						return TOKEN(TSUB);
<INITIAL>"*"						return TOKEN(TMUL);
<INITIAL>"/"						return TOKEN(TDIV);
<INITIAL>"%"						return TOKEN(TMOD);

<INITIAL>"@" {
	if_ignore_nl = !if_ignore_nl;
}

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
<INITIAL>{LETTER}({LETTER}|{DEC})* {
	SAVE_TOKEN();
	if (Ink_getParseEngine()->findProtocol(yylval.string->c_str())) {
		return TPROTOCOL;
	}
	return TIDENTIFIER;
}
<INITIAL>"0"{OCT}+\.{OCT}* { // oct
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>"0"{OCT}+ { // oct
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>"0"[xX]{HEX}+\.{HEX}* { // hex
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>"0"[xX]{HEX}+ { // hex
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>{DEC}*\.{DEC}* { // dec
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>0 { // dec
	SAVE_TOKEN();
	return TNUMERIC;
}
<INITIAL>[1-9]{DEC}* { // dec
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
	printf("EOF in comment\n");
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
<INITIAL>\' {
	id_literal = new std::string("", 0);
    BEGIN ID_LITERAL;
}
<ID_LITERAL>\' {
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
<ID_LITERAL>\\\"      *id_literal += '"';
<ID_LITERAL>\\'       *id_literal += '\'';
<ID_LITERAL>\\n       *id_literal += '\n';
<ID_LITERAL>\\t       *id_literal += '\t';
<ID_LITERAL>\\\\      *id_literal += '\\';
<ID_LITERAL><<EOF>>   {
	printf("EOF in id literal\n");
	yyterminate();
}
<ID_LITERAL>.         {
    *id_literal += yytext[0];
}

 /* String Literal */
<INITIAL>\" {
	string_literal = new std::string("", 0);
    BEGIN STRING_LITERAL;
}
<STRING_LITERAL>\" {
	yylval.string = string_literal;
	string_literal = NULL;
	BEGIN INITIAL;
	return TSTRING;
}
<STRING_LITERAL>\\{OCT}{1,3} {
	unsigned int letter;
	sscanf(&yytext[1], "%o", &letter);
    *string_literal += letter;
}
<STRING_LITERAL>\\[xX]{HEX}{1,2} {
	unsigned int letter;
	sscanf(&yytext[2], "%x", &letter);
    *string_literal += letter;
}
<STRING_LITERAL>{NEWLINE}        {
	*string_literal += yytext[0];
    LINE_NUMBER_INC();
}
<STRING_LITERAL>\\\"      *string_literal += '"';
<STRING_LITERAL>\\'       *string_literal += '\'';
<STRING_LITERAL>\\n       *string_literal += '\n';
<STRING_LITERAL>\\t       *string_literal += '\t';
<STRING_LITERAL>\\\\      *string_literal += '\\';
<STRING_LITERAL><<EOF>>   {
	printf("EOF in string literal\n");
	yyterminate();
}
<STRING_LITERAL>.         {
    *string_literal += yytext[0];
}

. {
	printf("Unknown token\n");
	yyterminate();
}

%%
