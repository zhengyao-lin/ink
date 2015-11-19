
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
%}

%option noyywrap
%start C_COMMENT CC_COMMENT

%%

<INITIAL>"var"						return TOKEN(TVAR);
<INITIAL>"null"						return TOKEN(TNULL);


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

. {
	printf("Unknown token\n");
	yyterminate();
}

%%
