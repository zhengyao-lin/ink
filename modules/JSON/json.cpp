#include <string>
#include <vector>
#include <algorithm>
#include "json.h"
#include "core/object.h"
#include "core/general.h"
#include "core/native/native.h"
#include "core/interface/engine.h"

using namespace ink;
using namespace std;

inline string JSON_escapeString(string str)
{
	string ret = "";
	string::size_type i;

	for (i = 0; i < str.length(); i++) {
		switch (str[i]) {
			case '\"':
				ret += "\\\"";
				break;
			case '\a':
				ret += "\\a";
				break;
			case '\b':
				ret += "\\b";
				break;
			case '\f':
				ret += "\\f";
				break;
			case '\n':
				ret += "\\n";
				break;
			case '\r':
				ret += "\\r";
				break;
			case '\t':
				ret += "\\t";
				break;
			case '\v':
				ret += "\\v";
				break;
			case '\\':
				ret += "\\\\";
				break;
			default:
				ret += str.substr(i, 1);
		}
	}

	return ret;
} 

string *JSON_stringifyObject(Ink_InterpreteEngine *engine, vector<Ink_Object *> trace, Ink_Object *obj)
{
	if (find(trace.begin(), trace.end(), obj) != trace.end()) {
		InkWarn_JSON_Cyclic_Reference(engine);
		return new string("null");
	}
	trace.push_back(obj);

	string ret = "", *tmp_str;
	Ink_Array *tmp_arr;
	Ink_ArrayValue tmp_arr_val;
	Ink_HashTable *hash_i;
	string::size_type i;

	if (!obj) return NULL;

	switch (obj->type) {
		case INK_ARRAY: {
			tmp_arr = as<Ink_Array>(obj);
			tmp_arr_val = tmp_arr->value;
			ret += "[";
			for (i = 0; i < tmp_arr_val.size(); i++) {
				if ((tmp_str = JSON_stringifyObject(engine, trace, tmp_arr_val[i]->getValue())) != NULL) {
					if (ret != "[") ret += ", ";
					ret += *tmp_str;
					delete tmp_str;
				}
			}
			ret += "]";
			break;
		} case INK_STRING: {
			ret += "\"" + JSON_escapeString(as<Ink_String>(obj)->getValue()) + "\"";
			break;
		} case INK_NULL: {
			ret += "null";
			break;
		}

		case INK_UNDEFINED:
		case INK_FUNCTION:
			break;
		
		default: {
			ret += "{";
			for (hash_i = obj->hash_table; hash_i;
				 hash_i = hash_i->next) {
				if (hash_i->getValue() && hash_i->getValue()->type != INK_UNDEFINED
					&& (tmp_str = JSON_stringifyObject(engine, trace, hash_i->getValue())) != NULL) {
					if (ret != "{") ret += ", ";
					ret += "\"" + string(hash_i->key) + "\"" + ": "
						   + *tmp_str;\
					delete tmp_str;
				}
			}
			ret += "}";
		}
	}

	return (ret != "") ? new string(ret) : NULL;
}

Ink_Object *InkNative_JSON_Encode(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 1)) {
		return NULL_OBJ;
	}

	string *tmp_str = JSON_stringifyObject(engine, vector<Ink_Object *>(), argv[0]);
	Ink_Object *ret;

	if (tmp_str)
		ret = new Ink_String(engine, tmp_str);
	else ret = NULL_OBJ;

	return ret;
}

void InkMod_JSON_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot_c("encode", new Ink_FunctionObject(engine, InkNative_JSON_Encode));
	bondee->setSlot_c("decode", new Ink_FunctionObject(engine, InkNative_JSON_Decode));

	return;
}

Ink_Object *InkMod_JSON_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *apply_to = argv[1];
	Ink_Object *json_pkg = addPackage(engine, apply_to, "json", new Ink_FunctionObject(engine, InkMod_JSON_Loader));

	InkMod_JSON_bondTo(engine, json_pkg);

	return NULL_OBJ;
}

Ink_ModuleID ink_native_json_mod_id;

extern "C" {
	void InkMod_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
	{
		addPackage(engine, context, "json", new Ink_FunctionObject(engine, InkMod_JSON_Loader));
		
		return;
	}

	int InkMod_Init(Ink_ModuleID id)
	{
		ink_native_json_mod_id = id;
		return 0;
	}
}
