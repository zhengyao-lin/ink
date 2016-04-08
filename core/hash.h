#ifndef _HASH_H_
#define _HASH_H_

#include <string>
#include <stdio.h>
#include "constant.h"

namespace ink {

class Ink_Object;
class Ink_InterpreteEngine;

class Ink_HashTable {
	enum {
		HASH_UNDEFINED,
		HASH_OBJ,
		HASH_CONST
	} type;
	
	union {
		Ink_Object *value;
		struct {
			Ink_Constant *value;
			Ink_InterpreteEngine *engine;
		} const_value;
	} u;

	Ink_Object *setter;
	Ink_Object *getter;

	Ink_HashTable *bonding;
	Ink_HashTable *bondee;

public:
	const char *key;

	Ink_HashTable *next;

	Ink_Object *parent;

	std::string *key_p;

	Ink_HashTable(const char *key, Ink_Object *value, Ink_Object *p, string *k_p = NULL);

	Ink_HashTable(const char *key, Ink_InterpreteEngine *engine, Ink_Constant *value, Ink_Object *p, std::string *key_p = NULL);

	Ink_HashTable(Ink_Object *value, Ink_Object *p);

	Ink_HashTable *getEnd();
	// Ink_HashTable *getMapping(const char *key);
	Ink_Object *getValue();
	Ink_Object *setValue(Ink_Object *val);
	Ink_Object *setValue(Ink_InterpreteEngine *engine, Ink_Constant *val);

	void setSetter(Ink_Object *obj);
	Ink_Object *getSetter();

	void setGetter(Ink_Object *obj);
	Ink_Object *getGetter();

	inline void setParent(Ink_Object *p)
	{
		parent = p;
		return;
	}

	inline Ink_Object *getParent()
	{
		return parent;
	}

	inline bool isConstant()
	{
		return type == HASH_CONST;
	}

	inline void initValue()
	{
		u.const_value.value = NULL;
		u.const_value.engine = NULL;
		return;
	}

	inline void cleanConst()
	{
		if (type == HASH_CONST)
			delete u.const_value.value;
		return;
	}

	void setConstant();
	inline void setUndefined()
	{
		cleanConst();
		type = HASH_UNDEFINED;
		initValue();
		return;
	}

	void setBonding(Ink_InterpreteEngine *engine, Ink_HashTable *to, bool if_remove = true);
	
	inline Ink_HashTable *getBonding()
	{
		return bonding;
	}

	inline void setBondee(Ink_HashTable *from)
	{
		bondee = from;
		return;
	}

	inline Ink_HashTable *getBondee()
	{
		return bondee;
	}

	~Ink_HashTable();
};

}

#endif
