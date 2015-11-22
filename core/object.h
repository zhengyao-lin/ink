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
	INK_INTEGER,
	INK_STRING,
	INK_FLOAT,
	INK_CONTEXT,
	INK_FUNCTION
};

void IGC_addObject(Ink_Object *obj);

extern bool CGC_if_return;

class Ink_Expression;
class Ink_ContextChain;
typedef vector<Ink_Expression *> Ink_ExpressionList;
typedef vector<string *> Ink_ParamList;

class Ink_Object {
public:
	bool marked;
	Ink_TypeTag type;

	Ink_HashTable *hash_table;

	Ink_Object **address;
	Ink_Object *base;

	Ink_Object(bool if_init_method = false)
	{
		marked = false;
		type = INK_OBJECT;
		hash_table = NULL;
		address = NULL;

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
	Ink_HashTable *setSlot(const char *key, Ink_Object *value);
	void cleanHashTable();
	void cleanHashTable(Ink_HashTable *table);
	void cloneHashTable(Ink_Object *src, Ink_Object *dest);

	virtual Ink_Object *clone();
	virtual Ink_Object *call(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
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
	{
		type = INK_UNDEFINED;
	}

	virtual Ink_Object *clone()
	{ return this; }

	virtual Ink_Object *call(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
	{
		InkErr_Calling_Undefined_Object(context);
		return NULL;
	}
};

class Ink_NullObject: public Ink_Object {
public:
	Ink_NullObject()
	{
		type = INK_NULL;
	}

	virtual Ink_Object *clone()
	{ return this; }
};

class Ink_ContextObject: public Ink_Object {
public:
	Ink_ContextObject()
	{
		type = INK_CONTEXT;
		initMethod();
	}
	Ink_ContextObject(Ink_HashTable *hash)
	{
		type = INK_CONTEXT;
		hash_table = hash;
		initMethod();
	}
};

typedef Ink_Object *(*Ink_NativeFunction)(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);

class Ink_FunctionObject: public Ink_Object {
public:
	bool is_native;
	Ink_NativeFunction native;

	Ink_HashTable *arguments;
	Ink_ExpressionList exp_list;

	Ink_ContextChain *closure_context;
	bool is_inline;

	Ink_FunctionObject(Ink_NativeFunction native)
	: is_native(true), native(native), arguments(NULL), exp_list(Ink_ExpressionList()), closure_context(NULL), is_inline(false)
	{ }

	Ink_FunctionObject(Ink_HashTable *arguments, Ink_ExpressionList exp_list)
	: is_native(false), native(NULL), arguments(arguments), exp_list(exp_list), closure_context(NULL), is_inline(false)
	{ initMethod(); }

	Ink_FunctionObject(Ink_ParamList param, Ink_ExpressionList exp_list, Ink_ContextChain *closure_context = NULL, bool is_inline = false)
	: is_native(false), native(NULL), exp_list(exp_list), closure_context(closure_context), is_inline(is_inline)
	{
		int i;
		Ink_HashTable *tmp = NULL;

		type = INK_FUNCTION;
		arguments = NULL;

		for (i = 0; i < (int)param.size(); i++) {
			if (arguments) {
				if (!tmp) tmp = arguments;
				tmp->next = new Ink_HashTable(param[i]->c_str(), NULL);
				tmp = tmp->next;
			} else {
				arguments = new Ink_HashTable(param[i]->c_str(), NULL);
			}
		}
		initMethod();
	}

	virtual Ink_Object *call(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
	virtual Ink_Object *clone()
	{
		//return new Ink_FunctionObject(arguments, exp_list);
		return this;
	}

	virtual ~Ink_FunctionObject();
};

class Ink_Integer: public Ink_Object {
public:
	int value;

	Ink_Integer(int value = 0)
	: value(value)
	{
		type = INK_INTEGER;
		initMethod();
	}

	virtual void derivedMethodInit()
	{
		Ink_IntegerMethodInit();
	}
	void Ink_IntegerMethodInit();

	virtual Ink_Object *clone();
};

class Ink_String: public Ink_Object {
public:
	string value;

	Ink_String(string value)
	: value(value)
	{
		type = INK_STRING;
		initMethod();
	}

	virtual void derivedMethodInit()
	{
		Ink_StringMethodInit();
	}
	void Ink_StringMethodInit();

	virtual Ink_Object *clone();
};

template <class T> T *as(Ink_Object *obj)
{
	return dynamic_cast<T*>(obj);
}

#endif