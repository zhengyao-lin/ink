#ifndef _NATIVE_PARSER_H_
#define _NATIVE_PARSER_H_

#include <string>
#include <vector>
#include <stack>

using namespace std;

enum INKJSON_Token_tag {
	JT_LBRACE = 1,
	JT_RBRACE,
	JT_LBRACKET,
	JT_RBRACKET,
	JT_COLON,
	JT_COMMA,
	JT_QUOTA,
	JT_STRING,
	JT_NUMERIC,
	JT_NULL
};
enum INKJSON_LexState {
	JLS_NORMAL,
	JLS_IN_STRING
};
enum INKJSON_ParseState {
	JPS_START,
	JPS_IN_HASH_TABLE,
	JPS_IN_ARRAY,
	JPS_IN_HASH
};

class INKJSON_Token;

typedef union INKJSON_Value_tag {
	string *str;
	double num;
} INKJSON_Value;
typedef vector<INKJSON_Token> INKJSON_TokenStack;
typedef vector<INKJSON_ParseState> INKJSON_ParseStateStack;

class INKJSON_Token {
public:
	INKJSON_Token_tag token;
	INKJSON_Value value;

	INKJSON_Token(INKJSON_Token_tag token, INKJSON_Value value)
	: token(token), value(value)
	{ }
};

class JSON_ParserReturnVal {
public:
	Ink_Object *ret;
	unsigned int end_index;

	JSON_ParserReturnVal(Ink_Object *ret = NULL, unsigned int end_index = 0)
	: ret(ret), end_index(end_index)
	{ }
};

#endif
