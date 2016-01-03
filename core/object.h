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
using namespace std;

void IGC_addObject(Ink_Object *obj);

extern InterruptSignal CGC_interrupt_signal;

class Ink_Expression;
class Ink_ContextObject;
class Ink_ContextChain;
class IGC_CollectEngine;
typedef vector<Ink_Expression *> Ink_ExpressionList;
// typedef triple<string *, bool, bool> Ink_Parameter;
typedef vector<Ink_Parameter> Ink_ParamList;

class Ink_Object {
public:
	int mark;
	Ink_TypeTag type;

	Ink_HashTable *hash_table;
	Ink_HashTable *address;

	Ink_Object *base;
	IGC_CollectEngine *alloc_engine;
	const char *debug_name;

	Ink_Object()
	{
		mark = 0;
		type = INK_OBJECT;
		hash_table = NULL;
		address = NULL;
		base = NULL;
		alloc_engine = NULL;
		debug_name = NULL;

		IGC_addObject(this);
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

	void Ink_ObjectMethodInit();
	virtual void derivedMethodInit() { }
	void initMethod()
	{
		// Ink_ObjectMethodInit();
		// derivedMethodInit();
	}

	Ink_Object *getSlot(const char *key);
	Ink_HashTable *getSlotMapping(const char *key);
	Ink_HashTable *setSlot(const char *key, Ink_Object *value, bool if_check_exist = true);
	void deleteSlot(const char *key);
	void cleanHashTable();
	void cleanHashTable(Ink_HashTable *table);
	static void cloneHashTable(Ink_Object *src, Ink_Object *dest);

	virtual Ink_Object *clone();
	virtual Ink_Object *call(Ink_ContextChain *context, unsigned int argc = 0, Ink_Object **argv = NULL,
							 Ink_Object *this_p = NULL, bool if_return_this = true)
	{
		InkErr_Calling_Non_Function_Object();
		return NULL;
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
	Ink_Undefined()
	{ type = INK_UNDEFINED; }

	virtual Ink_Object *clone()
	{ return this; }

	virtual Ink_Object *call(Ink_ContextChain *context, unsigned int argc = 0, Ink_Object **argv = NULL,
							 Ink_Object *this_p = NULL, bool if_return_this = true)
	{
		InkErr_Calling_Undefined_Object();
		return NULL;
	}
};

class Ink_NullObject: public Ink_Object {
public:
	Ink_NullObject()
	{ type = INK_NULL; }

	virtual Ink_Object *clone()
	{ return this; }
};

class Ink_ContextObject: public Ink_Object {
public:
	Ink_Object *ret_val;

	Ink_ContextObject()
	{
		type = INK_CONTEXT;
		ret_val = NULL;
	}
	Ink_ContextObject(Ink_HashTable *hash)
	{
		type = INK_CONTEXT;
		hash_table = hash;
		ret_val = NULL;
	}
};

typedef Ink_Object *(*Ink_NativeFunction)(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p);

class Ink_FunctionAttribution {
public:
	int interrupt_signal_trap;

	Ink_FunctionAttribution(int trap = INTER_RETURN | INTER_BREAK | INTER_CONTINUE | INTER_DROP)
	: interrupt_signal_trap(trap)
	{ }

	inline bool hasTrap(InterruptSignal signal)
	{
		return hasSignal(interrupt_signal_trap, signal);
	}
};

class Ink_FunctionObject: public Ink_Object {
	inline Ink_Object *cloneWithPA(unsigned int argc, Ink_Object **argv,
										   bool if_delete_argv = false)
	{
		unsigned int back_argc = partial_applied_argc;
		Ink_Object **back_argv = partial_applied_argv;
		Ink_Object *tmp;

		partial_applied_argc = argc;
		partial_applied_argv = argv;

		tmp = clone();

		partial_applied_argc = back_argc;
		partial_applied_argv = back_argv;

		if (if_delete_argv)
			free(argv);

		return tmp;
	}

public:
	bool is_native;
	bool is_inline;
	bool is_generator;

	Ink_NativeFunction native;

	Ink_ParamList param;
	Ink_ExpressionList exp_list;

	Ink_ContextChain *closure_context;

	Ink_FunctionAttribution attr;

	unsigned int partial_applied_argc;
	Ink_Object **partial_applied_argv;

	Ink_FunctionObject()
	: is_native(false), is_inline(false), is_generator(false), native(NULL),
	  param(Ink_ParamList()), exp_list(Ink_ExpressionList()), closure_context(NULL),
	  attr(Ink_FunctionAttribution()), partial_applied_argc(0), partial_applied_argv(NULL)
	{ type = INK_FUNCTION; }

	Ink_FunctionObject(Ink_NativeFunction native, bool is_inline = false)
	: is_native(true), is_inline(is_inline), is_generator(false), native(native),
	  param(Ink_ParamList()), exp_list(Ink_ExpressionList()), closure_context(NULL),
	  attr(Ink_FunctionAttribution()), partial_applied_argc(0), partial_applied_argv(NULL)
	{
		type = INK_FUNCTION;
		if (is_inline) {
			setAttr(Ink_FunctionAttribution(INTER_DROP));
		}
	}

	Ink_FunctionObject(Ink_ParamList param, Ink_ExpressionList exp_list, Ink_ContextChain *closure_context,
					   bool is_inline = false, bool is_generator = false)
	: is_native(false), is_inline(is_inline), is_generator(is_generator), native(NULL),
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

	virtual void derivedMethodInit()
	{
		Ink_FunctionMethodInit();
	}
	void Ink_FunctionMethodInit();

	virtual Ink_Object *call(Ink_ContextChain *context, unsigned int argc = 0, Ink_Object **argv = NULL,
							 Ink_Object *this_p = NULL, bool if_return_this = true);
	virtual Ink_Object *clone();

	virtual ~Ink_FunctionObject();
};

typedef double Ink_NumericValue;

class Ink_Numeric: public Ink_Object {
public:
	Ink_NumericValue value;

	Ink_Numeric(Ink_NumericValue value = 0)
	: value(value)
	{ type = INK_NUMERIC; }

	virtual void derivedMethodInit()
	{
		Ink_NumericMethodInit();
	}
	void Ink_NumericMethodInit();

	virtual Ink_Object *clone();
};

class Ink_String: public Ink_Object {
public:
	string value;

	Ink_String(string value)
	: value(value)
	{ type = INK_STRING; }

	virtual void derivedMethodInit()
	{
		Ink_StringMethodInit();
	}
	void Ink_StringMethodInit();

	virtual Ink_Object *clone();
};

typedef vector<Ink_HashTable *> Ink_ArrayValue;

class Ink_Array: public Ink_Object {
public:
	Ink_ArrayValue value;

	Ink_Array(Ink_ArrayValue value = Ink_ArrayValue())
	: value(value)
	{ type = INK_ARRAY; }

	virtual void derivedMethodInit()
	{
		Ink_ArrayMethodInit();
	}
	void Ink_ArrayMethodInit();
	Ink_ArrayValue cloneArrayValue(Ink_ArrayValue val);

	virtual Ink_Object *clone();

	static void disposeArrayValue(Ink_ArrayValue val)
	{
		unsigned int i;
		for (i = 0; i < val.size(); i++) {
			if (val[i])
				delete val[i];
		}
		return;
	}

	virtual ~Ink_Array()
	{
		disposeArrayValue(value);
	}
};

class Ink_Unknown: public Ink_Object {
public:

	Ink_Unknown()
	{
		type = INK_UNKNOWN;
	}

	virtual Ink_Object *clone();
};

template <class T> T *as(Ink_Object *obj)
{
	return dynamic_cast<T*>(obj);
}

#endif