#ifndef _NATIVE_PARSER_H_
#define _NATIVE_PARSER_H_

#include <string>
#include <vector>
#include <stack>

using namespace ink;
using namespace std;

enum InkJSON_Token_tag {
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
enum InkJSON_LexState {
	JLS_NORMAL,
	JLS_IN_STRING
};
enum InkJSON_ParseState {
	JPS_START,
	JPS_IN_HASH_TABLE,
	JPS_IN_ARRAY,
	JPS_IN_HASH
};

class InkJSON_Token;

typedef union InkJSON_Value_tag {
	string *str;
	double num;
} InkJSON_Value;
typedef vector<InkJSON_Token> InkJSON_TokenStack;
typedef vector<InkJSON_ParseState> InkJSON_ParseStateStack;

class InkJSON_Token {
public:
	InkJSON_Token_tag token;
	InkJSON_Value value;

	InkJSON_Token(InkJSON_Token_tag token, InkJSON_Value value)
	: token(token), value(value)
	{ }
};

class JSON_ParserReturnVal {
public:
	Ink_Object *ret;
	InkJSON_TokenStack::size_type end_index;

	JSON_ParserReturnVal(Ink_Object *ret = NULL, InkJSON_TokenStack::size_type end_index = 0)
	: ret(ret), end_index(end_index)
	{ }
};

#endif
