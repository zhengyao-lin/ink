#include <string>
#include "json.h"
#include "core/object.h"
#include "core/general.h"
#include "core/native/native.h"

using namespace std;

string *JSON_stringifyObject(Ink_Object *obj)
{
	string ret = "", *tmp_str;
	Ink_Array *tmp_arr;
	Ink_ArrayValue tmp_arr_val;
	Ink_HashTable *hash_i;
	unsigned int i;

	if (!obj) return NULL;

	switch (obj->type) {
		case INK_ARRAY: {
			tmp_arr = as<Ink_Array>(obj);
			tmp_arr_val = tmp_arr->value;
			ret += "[";
			for (i = 0; i < tmp_arr_val.size(); i++) {
				if ((tmp_str = JSON_stringifyObject(tmp_arr_val[i]->getValue())) != NULL) {
					if (ret != "[") ret += ", ";
					ret += *tmp_str;
					delete tmp_str;
				}
			}
			ret += "]";
			break;
		} case INK_STRING: {
			ret += "\"" + as<Ink_String>(obj)->value + "\"";
			break;
		} case INK_NULL: {
			ret += "null";
			break;
		} case INK_UNDEFINED: {
			break;
		} default: {
			ret += "{";
			for (hash_i = obj->hash_table; hash_i;
				 hash_i = hash_i->next) {
				if ((tmp_str = JSON_stringifyObject(hash_i->getValue())) != NULL) {
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

Ink_Object *InkNative_JSON_Stringify(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(argc, 1)) {
		return NULL_OBJ;
	}

	string *tmp_str = JSON_stringifyObject(argv[0]);
	Ink_Object *ret;

	if (tmp_str)
		ret = new Ink_String(*StrPool_addStr(tmp_str));
	else ret = NULL_OBJ;

	return ret;
}

Ink_Object *InkMod_JSON_Loader(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *global_context = context->getGlobal()->context;

	global_context->setSlot("stringify", new Ink_FunctionObject(InkNative_JSON_Stringify));

	return NULL_OBJ;
}

extern "C" {
	void InkMod_Loader(Ink_ContextChain *context)
	{
		Ink_Object *json_obj = addPackage(context, "json", new Ink_FunctionObject(InkMod_JSON_Loader));
		json_obj->setSlot("stringify", new Ink_FunctionObject(InkNative_JSON_Stringify));
		return;
	}
}