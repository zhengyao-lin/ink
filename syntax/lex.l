
SP				[ \t\v\f]
NEWLINE			[\r\n]
PM				[+-]
DIGIT			[0-9]
HEX				[a-fA-F0-9]
OCT				[0-7]
LETTER			[a-zA-Z_]
ISUFFIX			(l|L|s|S|u|U)+
FSUFFIX			(l|L)

DIGITS			({DIGIT})+
SPS				({SP})+

%{
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "core/expression.h"
#include "grammar.hpp"

#define SAVE_TOKEN()		(yylval.string = new std::string(yytext, yyleng))
#define TOKEN(t)			(yylval.token = t)
#define LINE_NUMBER_INC()	(current_line_number++)

int current_line_number = 1;
string *id_literal = NULL;
string *string_literal = NULL;
%}

%option noyywrap
%start C_COMMENT CC_COMMENT ID_LITERAL STRING_LITERAL

%%

<INITIAL>"var"						return TOKEN(TVAR);
<INITIAL>"null"						return TOKEN(TNULL);
<INITIAL>"undefined"				return TOKEN(TUNDEFINED);
<INITIAL>"return"					return TOKEN(TRETURN);

<INITIAL>","						return TOKEN(TCOMMA);
<INITIAL>";"						return TOKEN(TSEMICOLON);
<INITIAL>"."						return TOKEN(TDOT);

<INITIAL>"="						return TOKEN(TASSIGN);

<INITIAL>"("						return TOKEN(TLPAREN);
<INITIAL>")"						return TOKEN(TRPAREN);
<INITIAL>"["						return TOKEN(TLBRAKT);
<INITIAL>"]"						return TOKEN(TRBRAKT);
<INITIAL>"{"						return TOKEN(TLBRACE);
<INITIAL>"}"						return TOKEN(TRBRACE);

<INITIAL>"+"						return TOKEN(TADD);
<INITIAL>"-"						return TOKEN(TSUB);
<INITIAL>"*"						return TOKEN(TMUL);
<INITIAL>"/"						return TOKEN(TDIV);
<INITIAL>"%"						return TOKEN(TMOD);

<INITIAL>{NEWLINE}					LINE_NUMBER_INC();
<INITIAL>{SP}						/* Blank */;

 /* Constants */
<INITIAL>{LETTER}({LETTER}|{DIGIT})* {
	SAVE_TOKEN();
	return TIDENTIFIER;
}
<INITIAL>{DIGIT}+\.{DIGIT}+{FSUFFIX}? {
	SAVE_TOKEN();
	return TFLOAT;
}
<INITIAL>"0"{OCT}+{ISUFFIX}? { // oct
	SAVE_TOKEN();
	return TINTEGER;
}
<INITIAL>"0"[xX]{HEX}+{ISUFFIX}? { // hex
	SAVE_TOKEN();
	return TINTEGER;
}
<INITIAL>{DIGIT}+{ISUFFIX}? { // dec
	SAVE_TOKEN();
	return TINTEGER;
}

 /* Comments */
<INITIAL>"/*"     							BEGIN C_COMMENT;
<INITIAL>"//"     							BEGIN CC_COMMENT;

<C_COMMENT>{NEWLINE} {
	LINE_NUMBER_INC();
}
<C_COMMENT>"*/"							BEGIN INITIAL;
<C_COMMENT><<EOF>> {
	printf("EOF in comment\n");
	exit(1);
}
<C_COMMENT>.								;
<CC_COMMENT>{NEWLINE} {
	LINE_NUMBER_INC();
	BEGIN INITIAL;
}
<CC_COMMENT><<EOF>>	BEGIN INITIAL;
<CC_COMMENT>.			;

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
