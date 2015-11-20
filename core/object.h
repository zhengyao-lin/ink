#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "inttype.h"
#include "hash.h"
using namespace std;

enum Ink_TypeTag {
	INK_NULL = 0,
	INK_UNDEFINED,

	INK_OBJECT,
	INK_INTEGER,
	INK_FLOAT,
	INK_CONTEXT,
	INK_FUNCTION
};

void IGC_addObject(Ink_Object *obj);

class Ink_Expression;
class Ink_ContextChain;
typedef vector<Ink_Expression *> Ink_ExpressionList;
typedef vector<string *> Ink_ParamList;

class Ink_Object {
public:
	bool marked = false;
	Ink_TypeTag type;

	Ink_HashTable *hash_table;

	Ink_Object **address;
	Ink_Object *base;

	Ink_Object()
	{
		type = INK_OBJECT;
		hash_table = NULL;
		address = NULL;

		IGC_addObject(this);
	}

	Ink_Object *getSlot(const char *key);
	Ink_HashTable *getSlotMapping(const char *key);
	Ink_HashTable *setSlot(const char *key, Ink_Object *value);
	void cleanHashTable();
	void cleanHashTable(Ink_HashTable *table);
	void cloneHashTable(Ink_Object *src, Ink_Object *dest);

	virtual Ink_Object *clone();
	virtual Ink_Object *call(Ink_ContextChain *context, int argc, Ink_Object **argv)
	{
		// TODO: Fatal Error: Calling non-function object
		abort();
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
};

class Ink_NullObject: public Ink_Object {
public:
	Ink_NullObject()
	{
		type = INK_NULL;
	}
};

class Ink_ContextObject: public Ink_Object {
public:
	Ink_ContextObject()
	{
		type = INK_CONTEXT;
	}
	Ink_ContextObject(Ink_HashTable *hash)
	{
		type = INK_CONTEXT;
		hash_table = hash;
	}
};

typedef Ink_Object *(*Ink_NativeFunction)(Ink_ContextChain *context, int argc, Ink_Object **argv);

class Ink_FunctionObject: public Ink_Object {
public:
	bool is_native = false;
	Ink_NativeFunction native = NULL;

	Ink_HashTable *arguments = NULL;
	Ink_ExpressionList exp_list;

	Ink_FunctionObject(Ink_NativeFunction native)
	: is_native(true), native(native), exp_list(Ink_ExpressionList())
	{ }

	Ink_FunctionObject(Ink_HashTable *arguments, Ink_ExpressionList exp_list)
	: arguments(arguments), exp_list(exp_list)
	{ }

	Ink_FunctionObject(Ink_ParamList param, Ink_ExpressionList exp_list)
	: exp_list(exp_list)
	{
		int i;
		Ink_HashTable *tmp;

		type = INK_FUNCTION;
		arguments = NULL;

		for (i = 0; i < param.size(); i++) {
			if (arguments) {
				if (!tmp) tmp = arguments;
				tmp->next = new Ink_HashTable(param[i]->c_str(), NULL);
				tmp = tmp->next;
			} else {
				arguments = new Ink_HashTable(param[i]->c_str(), NULL);
			}
		}
	}

	virtual Ink_Object *call(Ink_ContextChain *context, int argc, Ink_Object **argv);
	virtual Ink_Object *clone()
	{
		//return new Ink_FunctionObject(arguments, exp_list);
		return this; // TODO
	}

	virtual ~Ink_FunctionObject()
	{
		cleanHashTable(arguments);
	}
};

class Ink_Integer: public Ink_Object {
public:
	int value;

	Ink_Integer(int value = 0)
	: value(value)
	{
		type = INK_INTEGER;
		Ink_IntegerMethodInit();
	}

	void Ink_IntegerMethodInit();

	virtual Ink_Object *clone();
};

class Ink_Float: public Ink_Object {
public:
	double value;

	Ink_Float(double value = 0.0)
	: value(value)
	{ type = INK_FLOAT; }
};

template <class T> T *as(Ink_Object *obj)
{
	return dynamic_cast<T*>(obj);
}

#endif