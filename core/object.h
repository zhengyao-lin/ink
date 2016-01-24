#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "type.h"
#include "inttype.h"
#include "hash.h"
#include "error.h"
#include "general.h"
#include "numeric.h"
#include "coroutine/coroutine.h"
using namespace std;

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

	Ink_Object *base;
	IGC_CollectEngine *alloc_engine;
	const char *debug_name;

	Ink_Object(Ink_InterpreteEngine *engine)
	{
		mark = 0;
		type = INK_OBJECT;
		hash_table = NULL;
		address = NULL;
		base = NULL;
		alloc_engine = NULL;
		debug_name = NULL;

		if (engine)
			IGC_addObject(engine, this);
		// initMethod();
	}

	inline void setDebugName(const char *name)
	{
		debug_name = name;
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
	/*
	void initMethod()
	{
		// Ink_ObjectMethodInit();
		// derivedMethodInit();
	}
	*/
	Ink_Object *getSlot(Ink_InterpreteEngine *engine, const char *key);
	Ink_HashTable *getSlotMapping(Ink_InterpreteEngine *engine, const char *key);
	Ink_HashTable *setSlot(const char *key, Ink_Object *value, bool if_check_exist = true, string *key_p = NULL);
	inline Ink_HashTable *setSlot(const char *key, Ink_Object *value, string *key_p) {
		return setSlot(key, value, true, key_p);
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
	virtual Ink_Object *call(Ink_InterpreteEngine *engine,
							 Ink_ContextChain *context, Ink_ArgcType argc = 0, Ink_Object **argv = NULL,
							 Ink_Object *this_p = NULL, bool if_return_this = true)
	{
		InkErr_Calling_Non_Function_Object(engine);
		return NULL;
	}
	virtual void doSelfMark(Ink_InterpreteEngine *engine, IGC_Marker marker)
	{
		return;
	}

	virtual ~Ink_Object()
	{
		cleanHashTable();
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

	virtual Ink_Object *call(Ink_InterpreteEngine *engine,
							 Ink_ContextChain *context, Ink_ArgcType argc = 0, Ink_Object **argv = NULL,
							 Ink_Object *this_p = NULL, bool if_return_this = true)
	{
		InkErr_Calling_Undefined_Object(engine);
		return NULL;
	}
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
public:
	Ink_Object *ret_val;

	Ink_ContextObject(Ink_InterpreteEngine *engine)
	: Ink_Object(engine)
	{
		type = INK_CONTEXT;
		ret_val = NULL;
	}
	Ink_ContextObject(Ink_InterpreteEngine *engine, Ink_HashTable *hash)
	: Ink_Object(engine)
	{
		type = INK_CONTEXT;
		hash_table = hash;
		ret_val = NULL;
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
};

typedef Ink_Object *(*Ink_NativeFunction)(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc,
										  Ink_Object **argv, Ink_Object *this_p);

class Ink_FunctionAttribution {
public:
	Ink_InterruptSignalTrap interrupt_signal_trap;

	Ink_FunctionAttribution(Ink_InterruptSignalTrap trap = INTER_RETURN | INTER_BREAK | INTER_CONTINUE | INTER_DROP)
	: interrupt_signal_trap(trap)
	{ }

	inline bool hasTrap(InterruptSignal sig)
	{
		return hasSignal(interrupt_signal_trap, sig);
	}
};

class Ink_FunctionObject: public Ink_Object {
public:
	bool is_native;
	bool is_inline;
	bool is_generator;

	Ink_NativeFunction native;

	Ink_ParamList param;
	Ink_ExpressionList exp_list;

	Ink_ContextChain *closure_context;

	Ink_FunctionAttribution attr;

	Ink_ArgcType partial_applied_argc;
	Ink_Object **partial_applied_argv;

	Ink_FunctionObject(Ink_InterpreteEngine *engine)
	: Ink_Object(engine),
	  is_native(false), is_inline(false), is_generator(false), native(NULL),
	  param(Ink_ParamList()), exp_list(Ink_ExpressionList()), closure_context(NULL),
	  attr(Ink_FunctionAttribution()), partial_applied_argc(0), partial_applied_argv(NULL)
	{ type = INK_FUNCTION; }

	Ink_FunctionObject(Ink_InterpreteEngine *engine, Ink_NativeFunction native, bool is_inline = false)
	: Ink_Object(engine),
	  is_native(true), is_inline(is_inline), is_generator(false), native(native),
	  param(Ink_ParamList()), exp_list(Ink_ExpressionList()), closure_context(NULL),
	  attr(Ink_FunctionAttribution()), partial_applied_argc(0), partial_applied_argv(NULL)
	{
		type = INK_FUNCTION;
		if (is_inline) {
			setAttr(Ink_FunctionAttribution(INTER_DROP));
		}
	}

	Ink_FunctionObject(Ink_InterpreteEngine *engine, Ink_NativeFunction native, Ink_ParamList param)
	: Ink_Object(engine),
	  is_native(true), is_inline(false), is_generator(false), native(native),
	  param(param), exp_list(Ink_ExpressionList()), closure_context(NULL),
	  attr(Ink_FunctionAttribution()), partial_applied_argc(0), partial_applied_argv(NULL)
	{
		type = INK_FUNCTION;
		if (is_inline) {
			setAttr(Ink_FunctionAttribution(INTER_DROP));
		}
	}

	Ink_FunctionObject(Ink_InterpreteEngine *engine,
					   Ink_ParamList param, Ink_ExpressionList exp_list, Ink_ContextChain *closure_context,
					   bool is_inline = false, bool is_generator = false)
	: Ink_Object(engine),
	  is_native(false), is_inline(is_inline), is_generator(is_generator), native(NULL),
	  param(param), exp_list(exp_list), closure_context(closure_context), attr(Ink_FunctionAttribution()),
	  partial_applied_argc(0), partial_applied_argv(NULL)
	{
		type = INK_FUNCTION;
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

	virtual Ink_Object *call(Ink_InterpreteEngine *engine,
							 Ink_ContextChain *context, Ink_ArgcType argc = 0, Ink_Object **argv = NULL,
							 Ink_Object *this_p = NULL, bool if_return_this = true);
	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
	virtual bool isTrue()
	{
		return true;
	}
	inline Ink_Object *cloneWithPA(Ink_InterpreteEngine *engine,
								   Ink_ArgcType argc, Ink_Object **argv,
								   bool if_delete_argv = false)
	{
		Ink_ArgcType back_argc = partial_applied_argc;
		Ink_Object **back_argv = partial_applied_argv;
		Ink_Object *tmp;

		partial_applied_argc = argc;
		partial_applied_argv = argv;

		tmp = clone(engine);

		partial_applied_argc = back_argc;
		partial_applied_argv = back_argv;

		if (if_delete_argv)
			free(argv);

		return tmp;
	}
	virtual void doSelfMark(Ink_InterpreteEngine *engine, IGC_Marker marker);

	virtual ~Ink_FunctionObject();
};

typedef double Ink_NumericValue;

class Ink_Numeric: public Ink_Object {
public:
	Ink_NumericValue value;

	Ink_Numeric(Ink_InterpreteEngine *engine, Ink_NumericValue value = 0)
	: Ink_Object(engine), value(value)
	{ type = INK_NUMERIC; }

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_NumericMethodInit(engine);
	}
	void Ink_NumericMethodInit(Ink_InterpreteEngine *engine);

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
	virtual bool isTrue()
	{
		return value != 0.0;
	}
};

class Ink_BigNumeric: public Ink_Object {
public:
	Ink_BigNumericValue value;

	Ink_BigNumeric(Ink_InterpreteEngine *engine, string value)
	: Ink_Object(engine), value(Ink_BigNumericValue(value))
	{ type = INK_BIGNUMERIC; }

	Ink_BigNumeric(Ink_InterpreteEngine *engine, Ink_BigNumericValue value)
	: Ink_Object(engine), value(value)
	{ type = INK_BIGNUMERIC; }

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_BigNumericMethodInit(engine);
	}
	void Ink_BigNumericMethodInit(Ink_InterpreteEngine *engine);

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
	virtual bool isTrue()
	{
		return value != 0.0;
	}
};

class Ink_String: public Ink_Object {
	string *value;
public:

	Ink_String(Ink_InterpreteEngine *engine, string value)
	: Ink_Object(engine), value(new string(value))
	{ type = INK_STRING; }

	Ink_String(Ink_InterpreteEngine *engine, string *value)
	: Ink_Object(engine), value(value)
	{ type = INK_STRING; }

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_StringMethodInit(engine);
	}
	void Ink_StringMethodInit(Ink_InterpreteEngine *engine);

	inline string getValue()
	{
		return *value;
	}

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
	virtual bool isTrue()
	{
		return true;
	}

	virtual ~Ink_String()
	{
		delete value;
	}
};

typedef vector<Ink_HashTable *> Ink_ArrayValue;

class Ink_Array: public Ink_Object {
public:
	Ink_ArrayValue value;

	Ink_Array(Ink_InterpreteEngine *engine, Ink_ArrayValue value = Ink_ArrayValue())
	: Ink_Object(engine), value(value)
	{ type = INK_ARRAY; }

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_ArrayMethodInit(engine);
	}
	void Ink_ArrayMethodInit(Ink_InterpreteEngine *engine);
	
	Ink_ArrayValue cloneArrayValue(Ink_ArrayValue val);
	Ink_ArrayValue cloneDeepArrayValue(Ink_InterpreteEngine *engine, Ink_ArrayValue val);

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

	virtual void doSelfMark(Ink_InterpreteEngine *engine, IGC_Marker marker);

	virtual ~Ink_Array()
	{
		disposeArrayValue(value);
	}
};

class Ink_Unknown: public Ink_Object {
public:

	Ink_Unknown(Ink_InterpreteEngine *engine)
	: Ink_Object(engine)
	{
		type = INK_UNKNOWN;
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
bool InkCoCall_switchCoroutine(Ink_InterpreteEngine *engine);

template <class T> T *as(Ink_Object *obj)
{
	return dynamic_cast<T*>(obj);
}

#endif
