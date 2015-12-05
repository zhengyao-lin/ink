#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "inttype.h"
#include "hash.h"
#include "error.h"
using namespace std;

enum Ink_TypeTag {
	INK_NULL = 0,
	INK_UNDEFINED,

	INK_OBJECT,
	INK_NUMERIC,
	INK_STRING,
	INK_FLOAT,
	INK_CONTEXT,
	INK_FUNCTION,
	INK_ARRAY
};

void IGC_addObject(Ink_Object *obj);

extern bool CGC_if_return;

class Ink_Expression;
class Ink_ContextChain;
class IGC_CollectEngine;
typedef vector<Ink_Expression *> Ink_ExpressionList;
typedef vector<string *> Ink_ParamList;

class Ink_Object {
public:
	int marked;
	Ink_TypeTag type;

	Ink_HashTable *hash_table;
	Ink_HashTable *address;

	Ink_Object *base;

	Ink_Object(bool if_init_method = false)
	{
		marked = 0;
		type = INK_OBJECT;
		hash_table = NULL;
		address = NULL;
		base = NULL;

		IGC_addObject(this);
		if (if_init_method) initMethod();
	}

	void Ink_ObjectMethodInit();
	virtual void derivedMethodInit() { }
	void initMethod()
	{
		Ink_ObjectMethodInit();
		derivedMethodInit();
	}

	Ink_Object *getSlot(const char *key);
	Ink_HashTable *getSlotMapping(const char *key);
	Ink_HashTable *setSlot(const char *key, Ink_Object *value, bool if_check_exist = true);
	void deleteSlot(const char *key);
	void cleanHashTable();
	void cleanHashTable(Ink_HashTable *table);
	static void cloneHashTable(Ink_Object *src, Ink_Object *dest);

	virtual Ink_Object *clone();
	virtual Ink_Object *call(Ink_ContextChain *context, unsigned int argc = 0, Ink_Object **argv = NULL, Ink_Object *this_p = NULL)
	{
		InkErr_Calling_Non_Function_Object(context);
		return NULL;
	}

	virtual ~Ink_Object()
	{
		cleanHashTable();
	}
};

class Ink_Undefined: public Ink_Object {
public:
	Ink_Undefined()
	{ type = INK_UNDEFINED; }

	virtual Ink_Object *clone()
	{ return this; }

	virtual Ink_Object *call(Ink_ContextChain *context, unsigned int argc = 0, Ink_Object **argv = NULL, Ink_Object *this_p = NULL)
	{
		InkErr_Calling_Undefined_Object(context);
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
	Ink_ContextObject()
	{ type = INK_CONTEXT; }
	Ink_ContextObject(Ink_HashTable *hash)
	{
		type = INK_CONTEXT;
		hash_table = hash;
	}
};

typedef Ink_Object *(*Ink_NativeFunction)(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p);

class Ink_FunctionObject: public Ink_Object {
public:
	bool is_native;
	bool is_inline;

	Ink_NativeFunction native;

	Ink_ParamList param;
	Ink_ExpressionList exp_list;

	Ink_ContextChain *closure_context;

	Ink_FunctionObject()
	: is_native(false), is_inline(false), native(NULL), param(Ink_ParamList()), exp_list(Ink_ExpressionList()), closure_context(NULL)
	{ type = INK_FUNCTION; }

	Ink_FunctionObject(Ink_NativeFunction native, bool is_inline = false)
	: is_native(true), is_inline(is_inline), native(native), param(Ink_ParamList()), exp_list(Ink_ExpressionList()), closure_context(NULL)
	{ type = INK_FUNCTION; }

	Ink_FunctionObject(Ink_ParamList param, Ink_ExpressionList exp_list, Ink_ContextChain *closure_context = NULL, bool is_inline = false)
	: is_native(false), is_inline(is_inline), native(NULL), param(param), exp_list(exp_list), closure_context(closure_context)
	{ type = INK_FUNCTION; }

	virtual void derivedMethodInit()
	{
		Ink_FunctionMethodInit();
	}
	void Ink_FunctionMethodInit();

	virtual Ink_Object *call(Ink_ContextChain *context, unsigned int argc = 0, Ink_Object **argv = NULL, Ink_Object *this_p = NULL);
	virtual Ink_Object *clone();

	virtual ~Ink_FunctionObject();
};

class Ink_Numeric: public Ink_Object {
public:
	long double value;

	Ink_Numeric(long double value = 0)
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

	virtual ~Ink_Array()
	{
		unsigned int i;
		for (i = 0; i < value.size(); i++) {
			delete value[i];
		}
	}
};

template <class T> T *as(Ink_Object *obj)
{
	return dynamic_cast<T*>(obj);
}

#endif