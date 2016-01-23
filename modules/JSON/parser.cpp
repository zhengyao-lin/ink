#include <stdio.h>
#include "json.h"
#include "parser.h"
#include "core/object.h"
#include "core/general.h"
#include "core/error.h"
#include "core/native/native.h"
#include "core/expression.h"
#include "core/interface/engine.h"

InkJSON_TokenStack
JSON_lexer(string str)
{
	InkJSON_TokenStack ret = InkJSON_TokenStack();
	string::size_type i;
	InkJSON_LexState state = JLS_NORMAL;
	string string_literal = "";
	string numeric_literal = "";
	InkJSON_Value tmp_val;

	for (i = 0; i < str.length(); i++) {
		switch (str[i]) {
			case '{':
				switch (state) {
					case JLS_NORMAL:
						tmp_val.str = NULL;
						ret.push_back(InkJSON_Token(JT_LBRACE, tmp_val));
						break;
					case JLS_IN_STRING:
						string_literal += "{";
				}
				break;
			case '}':
				switch (state) {
					case JLS_NORMAL:
						tmp_val.str = NULL;
						ret.push_back(InkJSON_Token(JT_RBRACE, tmp_val));
						break;
					case JLS_IN_STRING:
						string_literal += "}";
				}
				break;
			case '[':
				switch (state) {
					case JLS_NORMAL:
						tmp_val.str = NULL;
						ret.push_back(InkJSON_Token(JT_LBRACKET, tmp_val));
						break;
					case JLS_IN_STRING:
						string_literal += "[";
				}
				break;
			case ']':
				switch (state) {
					case JLS_NORMAL:
						tmp_val.str = NULL;
						ret.push_back(InkJSON_Token(JT_RBRACKET, tmp_val));
						break;
					case JLS_IN_STRING:
						string_literal += "]";
				}
				break;
			case ':':
				switch (state) {
					case JLS_NORMAL:
						tmp_val.str = NULL;
						ret.push_back(InkJSON_Token(JT_COLON, tmp_val));
						break;
					case JLS_IN_STRING:
						string_literal += ":";
				}
				break;
			case ',':
				switch (state) {
					case JLS_NORMAL:
						tmp_val.str = NULL;
						ret.push_back(InkJSON_Token(JT_COMMA, tmp_val));
						break;
					case JLS_IN_STRING:
						string_literal += ",";
				}
				break;
			case '"':
				switch (state) {
					case JLS_NORMAL:
						state = JLS_IN_STRING;
						string_literal = "";
						break;
					case JLS_IN_STRING:
						state = JLS_NORMAL;
						tmp_val.str = new string(string_literal);
						ret.push_back(InkJSON_Token(JT_STRING, tmp_val));
						break;
				}
				break;
			case '\\':
				switch (state) {
					case JLS_NORMAL:
						break;
					case JLS_IN_STRING:
						if (i + 1 < str.length()) {
							switch (str[i + 1]) {
								case '"':
									string_literal += "\"";
									break;
								case 't':
									string_literal += "\t";
									break;
								case '\\':
									string_literal += "\\";
									break;
								default:
									i--;
							}
							i++;
						} else {
							fprintf(stderr, "JSON Lex: Unexpected token '\\', ignore\n");
						}
						break;
				}
				break;
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				if (state == JLS_IN_STRING) {
					string_literal += str.substr(i, 1);
					break;
				}
				numeric_literal = str.substr(i, 1);
				string::size_type j;
				bool is_success;
				double val;

				for (j = i + 1; j < str.length()
							&& ((str[j] >= '0'
							&& str[j] <= '9') || str[j] == '.'); j++) {
					numeric_literal += str.substr(j, 1);
				}
				i = j - 1;

				val = Ink_NumericConstant::parseNumeric(numeric_literal, &is_success);
				if (is_success) {
					tmp_val.num = val;
					ret.push_back(InkJSON_Token(JT_NUMERIC, tmp_val));
				} else {
					fprintf(stderr, "JSON Lex: Failed to parse numeric '%s'", numeric_literal.c_str());
				}
				break;
			}
			case '\n':
			case ' ':
			case '\t':
				if (state == JLS_IN_STRING) {
					string_literal += str.substr(i, 1);
					break;
				}
				break;
			case 'n':
				if (state == JLS_IN_STRING) {
					string_literal += str.substr(i, 1);
					break;
				}
				if (i + 3 < str.length() && str.substr(i, 4) == "null") {
					i += 3;
					ret.push_back(InkJSON_Token(JT_NULL, tmp_val));
					break;
				}
				// fallthrough
			default:
				switch (state) {
					case JLS_NORMAL:
						fprintf(stderr, "JSON Lex: Unexpected token '%c', ignore\n", str[i]);
						break;
					case JLS_IN_STRING:
						string_literal += str.substr(i, 1);
						break;
				}
		}
	}

	return ret;
}

JSON_ParserReturnVal
JSON_parser(Ink_InterpreteEngine *engine, InkJSON_TokenStack token_stack,
			InkJSON_TokenStack::size_type start_index = 0)
{
	Ink_Object *ret = NULL;
	JSON_ParserReturnVal tmp_ret;
	InkJSON_ParseStateStack state_stack = InkJSON_ParseStateStack();
	InkJSON_TokenStack::size_type i = start_index, j;
	Ink_ArrayValue arr_val;
	string *tmp_str;

	switch (token_stack[i].token) {
		case JT_LBRACE: {
			ret = new Ink_Object(engine);
			if (token_stack[i + 1].token == JT_RBRACE) {
				return JSON_ParserReturnVal(ret, i + 1);
			}
			for (j = i + 1;
				 j + 3 < token_stack.size();) {
				if (token_stack[j].token == JT_STRING
					&& token_stack[j + 1].token == JT_COLON) {
					tmp_str = token_stack[j].value.str;
					tmp_ret = JSON_parser(engine, token_stack, j + 2);
					j = tmp_ret.end_index;

					if (tmp_ret.ret) {
						ret->setSlot(tmp_str->c_str(), tmp_ret.ret, tmp_str);
						if (token_stack[tmp_ret.end_index + 1].token == JT_RBRACE) {
							return JSON_ParserReturnVal(ret, j + 1);
						} else if (token_stack[tmp_ret.end_index + 1].token == JT_COMMA) {
							j += 2;
							continue;
						}
					} else {
						return JSON_ParserReturnVal(NULL, j);
					}
				} else {
					fprintf(stderr, "Unexpected %d and %d, expecting string constant\n",
							token_stack[j].token, token_stack[j + 1].token);
					return JSON_ParserReturnVal(NULL, j);
				}
			}
			fprintf(stderr, "Unexpected ending, expecting brace\n");
			return JSON_ParserReturnVal(NULL, i);
		}
		case JT_LBRACKET: {
			arr_val = Ink_ArrayValue();
			if (token_stack[i + 1].token == JT_RBRACKET) {
				return JSON_ParserReturnVal(new Ink_Array(engine, arr_val), i + 1);
			}
			for (j = i + 1;
				 j + 1 < token_stack.size();) {
				tmp_ret = JSON_parser(engine, token_stack, j);
				j = tmp_ret.end_index;

				if (tmp_ret.ret) {
					arr_val.push_back(new Ink_HashTable(tmp_ret.ret));
					if (token_stack[tmp_ret.end_index + 1].token == JT_RBRACKET) {
						return JSON_ParserReturnVal(new Ink_Array(engine, arr_val), j + 1);
					} else if (token_stack[tmp_ret.end_index + 1].token == JT_COMMA) {
						j += 2;
						continue;
					} else {
						fprintf(stderr, "Unexpected %d, expecting comma or bracket\n",
								token_stack[j + 1].token);
						cleanArrayHashTable(arr_val);
						return JSON_ParserReturnVal(NULL, j);
					}
				} else {
					cleanArrayHashTable(arr_val);
					return JSON_ParserReturnVal(NULL, j);
				}
			}
			cleanArrayHashTable(arr_val);
			fprintf(stderr, "Unexpected ending, expecting bracket\n");
			return JSON_ParserReturnVal(NULL, i);
		}
		case JT_STRING:
			return JSON_ParserReturnVal(new Ink_String(engine, token_stack[i].value.str), i);
		case JT_NUMERIC:
			return JSON_ParserReturnVal(new Ink_Numeric(engine, token_stack[i].value.num), i);
		case JT_NULL:
			return JSON_ParserReturnVal(NULL_OBJ, i);
		default: ;
			//fprintf(stderr, "Unexpected %d, expecting ending\n", token_stack[i].token);
	}

	return JSON_ParserReturnVal(NULL, i);
}

Ink_Object *JSON_parse(Ink_InterpreteEngine *engine, string str)
{
	InkJSON_TokenStack token_stack = JSON_lexer(str);
	JSON_ParserReturnVal ret_val = JSON_parser(engine, token_stack);

	if (ret_val.end_index + 1 < token_stack.size()) {
		fprintf(stderr, "Unexpected %d, expecting ending\n", token_stack[ret_val.end_index + 1].token);
		return NULL_OBJ;
	}

	if (ret_val.ret) {
		return ret_val.ret;
	}

	return NULL_OBJ;
}

Ink_Object *InkNative_JSON_Decode(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return UNDEFINED;
	}

	return JSON_parse(engine, as<Ink_String>(argv[0])->getValue());
}
