#include <sstream>
#include "../object.h"
#include "../context.h"
#include "native.h"

namespace ink {

using namespace std;

inline wstring::size_type getRealIndex(Ink_SInt64 index, wstring::size_type size)
{
	while (index < 0) index += size;
	return index;
}

inline wstring::size_type getRealIndex(Ink_NumericValue val, wstring::size_type size)
{
	Ink_SInt64 index = getInt(val);
	while (index < 0) index += size;
	return index;
}

Ink_Object *InkNative_String_Add(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_String *tmp;

	ASSUME_BASE_TYPE(engine, INK_STRING);

	if (!checkArgument(engine, argc, 1)) {
		return NULL_OBJ;
	}

	if ((tmp = getStringVal(engine, context, argv[0])) != NULL) {
		return new Ink_String(engine, as<Ink_String>(base)->getWValue() + tmp->getWValue());
	}

	InkWarn_Invalid_Argument_For_String_Add(engine, argv[0]->type);
	return NULL_OBJ;
}

Ink_Object *InkNative_String_Index(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	wstring base_str;
	wstring::size_type index;

	ASSUME_BASE_TYPE(engine, INK_STRING);

	if (argc > 1) {
		/* two or more arguments -- slice */
		InkNote_Method_Fallthrough(engine, "[]", "slice", INK_STRING);
		return InkNative_String_Slice(engine, context, argc, argv, this_p);
	}

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		InkNote_Method_Fallthrough(engine, "[]", INK_STRING, INK_OBJECT);
		return InkNative_Object_Index(engine, context, argc, argv, this_p);
	}

	base_str = as<Ink_String>(base)->getWValue();
	index = getRealIndex(as<Ink_Numeric>(argv[0])->getValue(), base_str.length());

	if (index >= base_str.length()) {
		InkWarn_String_Index_Exceed(engine, index, base_str.length());
		return NULL_OBJ;
	}

	return new Ink_String(engine, base_str.substr(index, 1));
}

Ink_Object *InkNative_String_Char(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);

	wstring base_val = as<Ink_String>(base)->getWValue();
	wstring::size_type i = 0;

	if (argc && argv[0]->type == INK_NUMERIC) {
		i = getInt(as<Ink_Numeric>(argv[0])->getValue());
	}

	if (i >= base_val.length()) {
		InkWarn_String_Index_Exceed(engine, i, base_val.length());
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, base_val[i]);
}

Ink_Object *InkNative_String_Length(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);

	return new Ink_Numeric(engine, (Ink_SInt64)as<Ink_String>(base)->getWValue().length());
}

Ink_Object *InkNative_String_SubStr(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	wstring::size_type offset;
	wstring::size_type length;

	ASSUME_BASE_TYPE(engine, INK_STRING);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	wstring origin = as<Ink_String>(base)->getWValue();
	if (argc > 1 && argv[1]->type == INK_NUMERIC) {
		offset = getRealIndex(as<Ink_Numeric>(argv[0])->getValue(), origin.length());
		length = getInt(as<Ink_Numeric>(argv[1])->getValue());
	} else {
		offset = getRealIndex(as<Ink_Numeric>(argv[0])->getValue(), origin.length());
		length = string::npos;
	}

	if (offset >= origin.length()) {
		InkWarn_String_Index_Exceed(engine, offset, origin.length());
		return NULL_OBJ;
	} else if (!(length == string::npos || offset + length <= origin.length())) {
		InkWarn_Sub_String_Exceed(engine);
		return NULL_OBJ;
	}

	return new Ink_String(engine, origin.substr(offset, length));
}

Ink_Object *InkNative_String_Split(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	wstring::size_type i, last;

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	wstring base_str = as<Ink_String>(base)->getWValue();
	wstring split = as<Ink_String>(argv[0])->getWValue();
	Ink_ArrayValue ret_val = Ink_ArrayValue();
	wstring tmp;

	for (i = 0, last = 0; i < base_str.size(); i++) {
		if (base_str.substr(i, split.length()) == split) {
			ret_val.push_back(new Ink_HashTable(new Ink_String(engine, tmp = (i > last
																	   ? base_str.substr(last, i - last)
																	   : L""))));
			last = i + split.length();
			i += split.length() - 1;
		}
	}

	ret_val.push_back(new Ink_HashTable(new Ink_String(engine, i > last
															   ? base_str.substr(last, i - last)
															   : L"")));

	return new Ink_Array(engine, ret_val);
}

Ink_Object *InkNative_String_Slice(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);

	wstring base_val = as<Ink_String>(base)->getWValue();
	wstring::size_type start = 0, end = base_val.length() - 1, tmp, i;
	Ink_SInt64 range = 1;

	if (argc > 0) {
		if (argv[0]->type == INK_NUMERIC) {
			start = getRealIndex(as<Ink_Numeric>(argv[0])->getValue(), base_val.length());
		} else if (argv[0]->type != INK_UNDEFINED) {
			InkWarn_Slice_Require_Numeric(engine, 1);
		}

		if (argc > 1) {
			if (argv[1]->type == INK_NUMERIC) {
				end = getRealIndex(as<Ink_Numeric>(argv[1])->getValue(), base_val.size());
			} else if (argv[1]->type != INK_UNDEFINED) {
				InkWarn_Slice_Require_Numeric(engine, 2);
			}

			if (argc > 2) {
				if (argv[2]->type == INK_NUMERIC) {
					range = getInt(as<Ink_Numeric>(argv[2])->getValue());
				} else if (argv[2]->type != INK_UNDEFINED) {
					InkWarn_Slice_Require_Numeric(engine, 3);
				}
			}
		}
	}

	if (start > end) {
		InkNote_Slice_Start_Greater(engine, start, end);
		tmp = start;
		start = end;
		end = tmp;
	}

	if (!range) {
		InkWarn_Slice_Require_Non_Zero_Range(engine);
		range = 1;
	}

	if (end >= base_val.size()) {
		InkWarn_String_Index_Exceed(engine, end, base_val.size());
		return NULL_OBJ;
	}

	wchar_t *buf = (wchar_t *)malloc(sizeof(wchar_t) * (base_val.length() + 1));
	wstring::size_type buf_i;

	if (range > 0) {
		for (i = start, buf_i = 0; i <= end; i += range) {
			buf[buf_i++] = base_val[i];
		}
	} else {
		for (i = end, buf_i = 0; i >= start; i += range) {
			buf[buf_i++] = base_val[i];

			if ((Ink_UInt64)-range > i) {
				break;
			}
		}
	}

	wstring ret_val = wstring(buf, buf_i);
	Ink_String *ret = new Ink_String(engine, ret_val);
	free(buf);

	return ret;
}

Ink_Object *InkNative_String_Greater(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_String>(base)->getWValue()[0]
								   > as<Ink_String>(argv[0])->getWValue()[0]);
}

Ink_Object *InkNative_String_Less(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_String>(base)->getWValue()[0]
								   < as<Ink_String>(argv[0])->getWValue()[0]);
}

Ink_Object *InkNative_String_GreaterOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_String>(base)->getWValue()[0]
								   >= as<Ink_String>(argv[0])->getWValue()[0]);
}

Ink_Object *InkNative_String_LessOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_String>(base)->getWValue()[0]
								   <= as<Ink_String>(argv[0])->getWValue()[0]);
}

Ink_Object *InkNative_String_ToString(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	ASSUME_BASE_TYPE(engine, INK_STRING);
	return base;
}

extern int string_native_method_table_count;
extern InkNative_MethodTable string_native_method_table[];

void Ink_String::Ink_StringMethodInit(Ink_InterpreteEngine *engine)
{
	setSlot_c("+", new Ink_FunctionObject(engine, InkNative_String_Add));
	setSlot_c("<", new Ink_FunctionObject(engine, InkNative_String_Index));
	setSlot_c(">", new Ink_FunctionObject(engine, InkNative_String_Greater));
	setSlot_c("<", new Ink_FunctionObject(engine, InkNative_String_Less));
	setSlot_c(">=", new Ink_FunctionObject(engine, InkNative_String_GreaterOrEqual));
	setSlot_c("<=", new Ink_FunctionObject(engine, InkNative_String_LessOrEqual));
	setSlot_c("[]", new Ink_FunctionObject(engine, InkNative_String_Index));
	setSlot_c("char", new Ink_FunctionObject(engine, InkNative_String_Char));
	setSlot_c("length", new Ink_FunctionObject(engine, InkNative_String_Length));
	setSlot_c("substr", new Ink_FunctionObject(engine, InkNative_String_SubStr));
	setSlot_c("split", new Ink_FunctionObject(engine, InkNative_String_Split));
	setSlot_c("slice", new Ink_FunctionObject(engine, InkNative_String_Slice));
	setSlot_c("to_str", new Ink_FunctionObject(engine, InkNative_String_ToString));

	return;
}

}
