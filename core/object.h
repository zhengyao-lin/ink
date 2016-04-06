#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <string.h>
#include <vector>
#include <string>
#include "type.h"
#include "hash.h"
#include "general.h"
#include "constant.h"
#include "error.h"
#include "utf8.h"
#include "coroutine/coroutine.h"

namespace ink {

class Ink_Expression;
class Ink_ContextObject;
class Ink_ContextChain;
class IGC_CollectEngine;
class Ink_InterpreteEngine;

void IGC_addObject(Ink_InterpreteEngine *current_engine, Ink_Object *obj);

class Ink_Object {
public:
	IGC_MarkType mark;
	Ink_TypeTag type;

	Ink_HashTable *hash_table;
	Ink_HashTable *address;

	char *debug_name;

	Ink_InterpreteEngine *engine;

	Ink_HashTable *proto_hash;
	Ink_Object *base_p;

	Ink_Object(Ink_InterpreteEngine *engine)
	: engine(engine)
	{
		mark = MARK_WHITE; // IGC_Mark_White;
		type = INK_OBJECT;
		hash_table = NULL;
		address = NULL;
		debug_name = NULL;
		proto_hash = NULL;
		base_p = NULL;
		
		initProto(engine);

		if (engine)
			IGC_addObject(engine, this);
		// initMethod();
	}

	void setBase(Ink_Object *obj);

	inline Ink_Object *getBase()
	{
		return base_p;
	}

	static inline Ink_HashTable *traceHashBond(Ink_HashTable *begin, bool set_bondee = true)
	{
		// return begin;
#if 1
		Ink_HashTable *ret = NULL;

		if (!begin) return NULL;

		for (ret = begin; ret->bonding; ret = ret->bonding) ;
		if (set_bondee)
			ret->bondee = begin;

		return ret;
#endif
	}

	void initProto(Ink_InterpreteEngine *engine);

	void setProto(Ink_Object *proto);

	inline Ink_HashTable *getProtoHash(bool set_bondee = true)
	{
		return traceHashBond(proto_hash, set_bondee);
	}

	Ink_Object *getProto();

	inline void setDebugName(const char *name)
	{
		if (debug_name)
			free(debug_name);
		debug_name = name ? strdup(name) : NULL;
		return;
	}

	inline const char *getDebugName()
	{
		return debug_name;
	}

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_ObjectMethodInit(engine);
	}
	void Ink_ObjectMethodInit(Ink_InterpreteEngine *engine);

	Ink_Object *getSlot(Ink_InterpreteEngine *engine, const char *key, bool search_prototype = true);
	Ink_HashTable *getSlotMapping(Ink_InterpreteEngine *engine, const char *key, bool *is_from_proto = NULL, bool search_prototype = true);
	inline Ink_HashTable *getSlotMapping(Ink_InterpreteEngine *engine, const char *key, bool search_prototype) {
		return getSlotMapping(engine, key, NULL, search_prototype);
	}

	Ink_HashTable *setSlot(const char *key, Ink_Object *value, bool if_check_exist = true, bool if_alloc_key = true);
	Ink_HashTable *setSlot(const char *key, Ink_InterpreteEngine *engine, Ink_Constant *value, bool if_check_exist = true, bool if_alloc_key = true);

	inline Ink_HashTable *setSlot_c(const char *key, Ink_Object *value, bool if_check_exist = true)
	{
		return setSlot(key, value, if_check_exist, false);
	}

	inline Ink_HashTable *setSlot_c(const char *key, Ink_InterpreteEngine *engine, Ink_Constant *value, bool if_check_exist = true)
	{
		return setSlot(key, engine, value, if_check_exist, false);
	}

	void deleteSlot(const char *key);
	void cleanHashTable();
	void cleanHashTable(Ink_HashTable *table);
	static void cloneHashTable(Ink_Object *src, Ink_Object *dest);
	static void cloneDeepHashTable(Ink_InterpreteEngine *engine, Ink_Object *src, Ink_Object *dest);

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
	virtual bool isTrue()
	{
		return true;
	}

	Ink_Object *triggerCallEvent(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
								 Ink_ArgcType argc, Ink_Object **argv);
	
	virtual Ink_Object *call(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
							 Ink_Object *base, Ink_ArgcType argc = 0, Ink_Object **argv = NULL,
							 Ink_Object *this_p = NULL, bool if_return_this = true);

	inline Ink_Object *call(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
							Ink_ArgcType argc = 0, Ink_Object **argv = NULL,
							Ink_Object *this_p = NULL, bool if_return_this = true)
	{
		return call(engine, context, getBase(), argc, argv, this_p, if_return_this);
	}

	virtual void doSelfMark(Ink_InterpreteEngine *engine, IGC_Marker marker)
	{
		return;
	}

	virtual Ink_Constant *toConstant(Ink_InterpreteEngine *engine)
	{
		return NULL;
	}

	virtual ~Ink_Object()
	{
		cleanHashTable();
		free(debug_name);
	}
};

inline bool isUnknown(Ink_Object *obj)
{
	return obj && obj->type == INK_UNKNOWN;
}

class Ink_Undefined: public Ink_Object {
public:
	Ink_Undefined(Ink_InterpreteEngine *engine)
	: Ink_Object(engine)
	{ type = INK_UNDEFINED; }

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine)
	{ return this; }

	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);

	virtual bool isTrue()
	{
		return false;
	}

	virtual Ink_Object *call(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
							 Ink_Object *base, Ink_ArgcType argc = 0, Ink_Object **argv = NULL,
							 Ink_Object *this_p = NULL, bool if_return_this = true);
};

class Ink_NullObject: public Ink_Object {
public:
	Ink_NullObject(Ink_InterpreteEngine *engine)
	: Ink_Object(engine)
	{ type = INK_NULL; }

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine)
	{ return this; }

	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);

	virtual bool isTrue()
	{
		return false;
	}
};

class Ink_ContextObject: public Ink_Object {
	Ink_Object *ret_val;

public:
	const char *debug_file_name;
	Ink_LineNoType debug_lineno;
	Ink_Object *debug_creater;

	Ink_ContextObject(Ink_InterpreteEngine *engine)
	: Ink_Object(engine)
	{
		type = INK_CONTEXT;
		ret_val = NULL;

		debug_file_name = NULL;
		debug_lineno = -1;
		debug_creater = NULL;
	}
	Ink_ContextObject(Ink_InterpreteEngine *engine, Ink_HashTable *hash)
	: Ink_Object(engine)
	{
		type = INK_CONTEXT;
		hash_table = hash;
		ret_val = NULL;

		debug_file_name = NULL;
		debug_lineno = -1;
		debug_creater = NULL;
	}

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
	virtual bool isTrue()
	{
		return true;
	}
	virtual void doSelfMark(Ink_InterpreteEngine *engine, IGC_Marker marker)
	{
		marker(engine, ret_val);
		return;
	}

	inline void setDebug(const char *file_name, Ink_LineNoType lineno, Ink_Object *creater)
	{
		debug_file_name = file_name;
		debug_lineno = lineno;
		debug_creater = creater;
		return;
	}

	inline const char *getFileName()
	{
		return debug_file_name;
	}

	inline Ink_LineNoType getLineno()
	{
		return debug_lineno;
	}

	inline Ink_Object *getCreater()
	{
		return debug_creater;
	}

	Ink_Object *setReturnVal(Ink_Object *obj);
	inline Ink_Object *getReturnVal()
	{
		return ret_val;
	}
};

typedef Ink_Object *(*Ink_NativeFunction)(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
										  Ink_Object *base, Ink_ArgcType argc,
										  Ink_Object **argv, Ink_Object *this_p);

class Ink_FunctionObject: public Ink_Object {
public:
	bool is_native;
	bool is_inline;
	bool is_ref;

	Ink_NativeFunction native;

	Ink_ParamList param;
	Ink_ExpressionList exp_list;

	Ink_ContextChain *closure_context;

	Ink_FunctionAttribution attr;

	bool is_pa;
	Ink_ArgcType pa_argc;
	Ink_Object **pa_argv;
	Ink_Object *pa_info_base_p;
	Ink_Object *pa_info_this_p;
	bool pa_info_if_return_this;

	Ink_FunctionObject(Ink_InterpreteEngine *engine)
	: Ink_Object(engine),
	  is_native(false), is_inline(false), is_ref(false), native(NULL),
	  param(Ink_ParamList()), exp_list(Ink_ExpressionList()), closure_context(NULL),
	  attr(Ink_FunctionAttribution()), is_pa(false), pa_argc(0), pa_argv(NULL), pa_info_base_p(NULL), pa_info_this_p(NULL),
	  pa_info_if_return_this(false)
	{
		type = INK_FUNCTION;
		initProto(engine);
	}

	Ink_FunctionObject(Ink_InterpreteEngine *engine, Ink_NativeFunction native, bool is_inline = false)
	: Ink_Object(engine),
	  is_native(true), is_inline(is_inline), is_ref(false), native(native),
	  param(Ink_ParamList()), exp_list(Ink_ExpressionList()), closure_context(NULL),
	  attr(Ink_FunctionAttribution()), is_pa(false), pa_argc(0), pa_argv(NULL), pa_info_base_p(NULL), pa_info_this_p(NULL),
	  pa_info_if_return_this(false)
	{
		type = INK_FUNCTION;
		initProto(engine);
		if (is_inline) {
			setAttr(Ink_FunctionAttribution(INTER_DROP));
		}
	}

	Ink_FunctionObject(Ink_InterpreteEngine *engine, Ink_NativeFunction native, Ink_ParamList param)
	: Ink_Object(engine),
	  is_native(true), is_inline(false), is_ref(false), native(native),
	  param(param), exp_list(Ink_ExpressionList()), closure_context(NULL),
	  attr(Ink_FunctionAttribution()), is_pa(false), pa_argc(0), pa_argv(NULL), pa_info_base_p(NULL), pa_info_this_p(NULL),
	  pa_info_if_return_this(false)
	{
		type = INK_FUNCTION;
		initProto(engine);
		if (is_inline) {
			setAttr(Ink_FunctionAttribution(INTER_DROP));
		}
	}

	Ink_FunctionObject(Ink_InterpreteEngine *engine,
					   Ink_ParamList param, Ink_ExpressionList exp_list, Ink_ContextChain *closure_context,
					   bool is_inline = false, bool is_ref = false)
	: Ink_Object(engine),
	  is_native(false), is_inline(is_inline), is_ref(is_ref), native(NULL),
	  param(param), exp_list(exp_list), closure_context(closure_context), attr(Ink_FunctionAttribution()),
	  is_pa(false), pa_argc(0), pa_argv(NULL), pa_info_base_p(NULL), pa_info_this_p(NULL), pa_info_if_return_this(false)
	{
		type = INK_FUNCTION;
		initProto(engine);
		if (is_inline) {
			setAttr(Ink_FunctionAttribution(INTER_DROP));
		}
	}

	inline void setAttr(Ink_FunctionAttribution a)
	{
		attr = a;
		return;
	}

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_FunctionMethodInit(engine);
	}
	void Ink_FunctionMethodInit(Ink_InterpreteEngine *engine);

	static void triggerInterruptEvent(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
									  Ink_ContextObject *local, Ink_Object *receiver);
	Ink_Object *checkUnkownArgument(Ink_Object *&base, Ink_ArgcType &argc, Ink_Object **&argv,
									Ink_Object *&this_p, bool &if_return_this, bool &if_delete_argv);

	virtual Ink_Object *call(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
							 Ink_Object *base, Ink_ArgcType argc = 0, Ink_Object **argv = NULL,
							 Ink_Object *this_p = NULL, bool if_return_this = true);
	
	inline Ink_Object *call(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
							Ink_ArgcType argc = 0, Ink_Object **argv = NULL,
							Ink_Object *this_p = NULL, bool if_return_this = true)
	{
		return call(engine, context, getBase(), argc, argv, this_p, if_return_this);
	}

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
	virtual bool isTrue()
	{
		return true;
	}
	inline Ink_Object *cloneWithPA(Ink_InterpreteEngine *engine, Ink_Object *base,
								   Ink_ArgcType argc, Ink_Object **argv,
								   Ink_Object *this_p, bool if_return_this,
								   bool if_delete_argv = false)
	{
		bool is_pa_back = is_pa;
		Ink_ArgcType argc_back = pa_argc;
		Ink_Object **argv_back = pa_argv;
		Ink_Object *base_p_back = pa_info_base_p;
		Ink_Object *this_p_back = pa_info_this_p;
		bool if_return_this_back = pa_info_if_return_this;
		Ink_Object *tmp;

		is_pa = true;
		pa_argc = argc;
		pa_argv = argv;
		pa_info_base_p = base;
		pa_info_this_p = this_p;
		pa_info_if_return_this = if_return_this;

		tmp = clone(engine);

		is_pa = is_pa_back;
		pa_argc = argc_back;
		pa_argv = argv_back;
		pa_info_base_p = base_p_back;
		pa_info_this_p = this_p_back;
		pa_info_if_return_this = if_return_this_back;

		if (if_delete_argv)
			free(argv);

		return tmp;
	}
	Ink_Object **copyDeepArgv(Ink_InterpreteEngine *engine, Ink_ArgcType argc, Ink_Object **argv);
	
	virtual void doSelfMark(Ink_InterpreteEngine *engine, IGC_Marker marker);

	virtual ~Ink_FunctionObject();
};

class Ink_ExpListObject: public Ink_Object {
public:
	Ink_ExpressionList exp_list;

	Ink_ExpListObject(Ink_InterpreteEngine *engine)
	: Ink_Object(engine), exp_list(Ink_ExpressionList())
	{
		type = INK_EXPLIST;
		initProto(engine);
	}

	Ink_ExpListObject(Ink_InterpreteEngine *engine, Ink_ExpressionList exp_list)
	: Ink_Object(engine), exp_list(exp_list)
	{
		type = INK_EXPLIST;
		initProto(engine);
	}

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_ExpListMethodInit(engine);
	}
	void Ink_ExpListMethodInit(Ink_InterpreteEngine *engine);

	virtual Ink_Object *call(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
							 Ink_Object *base, Ink_ArgcType argc = 0, Ink_Object **argv = NULL,
							 Ink_Object *this_p = NULL, bool if_return_this = true);

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
};

class Ink_Numeric: public Ink_Object {
	Ink_NumericValue value;
public:

	Ink_Numeric(Ink_InterpreteEngine *engine, Ink_NumericValue value)
	: Ink_Object(engine), value(value)
	{
		type = INK_NUMERIC;
		initProto(engine);
	}

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_NumericMethodInit(engine);
	}
	void Ink_NumericMethodInit(Ink_InterpreteEngine *engine);

	inline Ink_NumericValue getValue()
	{
		return value;
	}

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
	virtual bool isTrue()
	{
		return getBool(value);
	}

	virtual Ink_Constant *toConstant(Ink_InterpreteEngine *engine)
	{
		return new Ink_NumericConstant(value);
	}
};

class Ink_String: public Ink_Object {
	// std::string *value;
	std::wstring *value;
public:

	Ink_String(Ink_InterpreteEngine *engine, std::wstring v)
	: Ink_Object(engine), value(new std::wstring(v))
	{
		type = INK_STRING;
		initProto(engine);
	}

	Ink_String(Ink_InterpreteEngine *engine, std::wstring *v)
	: Ink_Object(engine), value(v)
	{
		type = INK_STRING;
		initProto(engine);
	}

	Ink_String(Ink_InterpreteEngine *engine, std::string v)
	: Ink_Object(engine)
	{
		type = INK_STRING;
		initProto(engine);
		wchar_t *tmp = Ink_mbstowcs_alloc(v.c_str());
		value = new std::wstring(tmp);
		free(tmp);
	}

	Ink_String(Ink_InterpreteEngine *engine, std::string *v)
	: Ink_Object(engine)
	{
		type = INK_STRING;
		initProto(engine);
		wchar_t *tmp = Ink_mbstowcs_alloc(v->c_str());
		value = new std::wstring(tmp);
		free(tmp);
		delete v;
	}

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_StringMethodInit(engine);
	}
	void Ink_StringMethodInit(Ink_InterpreteEngine *engine);

	inline std::string getValue()
	{
		char *tmp = Ink_wcstombs_alloc(value->c_str());
		std::string ret = std::string(tmp);
		free(tmp);
		return ret;
	}

	inline std::wstring getWValue()
	{
		return *value;
	}

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
	virtual bool isTrue()
	{
		return true;
	}

	virtual Ink_Constant *toConstant(Ink_InterpreteEngine *engine)
	{
		return new Ink_StringConstant(getWValue());
	}

	virtual ~Ink_String()
	{
		delete value;
	}
};

class Ink_Array: public Ink_Object {
public:
	Ink_ArrayValue value;

	Ink_Array(Ink_InterpreteEngine *engine, Ink_ArrayValue value = Ink_ArrayValue())
	: Ink_Object(engine), value(value)
	{
		type = INK_ARRAY;
		initProto(engine);
	}

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_ArrayMethodInit(engine);
	}
	void Ink_ArrayMethodInit(Ink_InterpreteEngine *engine);
	
	static Ink_ArrayValue cloneArrayValue(Ink_ArrayValue val, Ink_Object *parent);
	static Ink_ArrayValue cloneDeepArrayValue(Ink_InterpreteEngine *engine, Ink_ArrayValue val, Ink_Object *parent);

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
	virtual bool isTrue()
	{
		return true;
	}

	static void disposeArrayValue(Ink_ArrayValue val)
	{
		Ink_ArrayValue::size_type i;
		for (i = 0; i < val.size(); i++) {
			if (val[i])
				delete val[i];
		}
		return;
	}

	void disposeArrayValue();

	virtual void doSelfMark(Ink_InterpreteEngine *engine, IGC_Marker marker);

	virtual ~Ink_Array()
	{
		disposeArrayValue();
	}
};

class Ink_Unknown: public Ink_Object {
public:

	Ink_Unknown(Ink_InterpreteEngine *engine)
	: Ink_Object(engine)
	{
		type = INK_UNKNOWN;
		setProto(NULL);
	}

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine)
	{ return this; }

	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
	virtual bool isTrue()
	{
		return false;
	}
};

inline Ink_Object **copyArgv(Ink_ArgcType argc, Ink_Object **argv)
{
	Ink_Object **ret = (Ink_Object **)malloc(sizeof(Ink_Object *) * argc);
	memcpy(ret, argv, sizeof(Ink_Object *) * argc);
	return ret;
}

inline Ink_Object **linkArgv(Ink_ArgcType argc1, Ink_Object **argv1, Ink_ArgcType argc2, Ink_Object **argv2)
{
	Ink_Object **ret = (Ink_Object **)malloc(sizeof(Ink_Object *) * (argc1 + argc2));
	memcpy(ret, argv1, sizeof(Ink_Object *) * argc1);
	memcpy(&ret[argc1], argv2, sizeof(Ink_Object *) * argc2);
	return ret;
}

Ink_Object *InkCoCall_call(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_CoCallList call_list);

}

#endif
