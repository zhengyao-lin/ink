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
	};

public:
	const char *key;

	Ink_HashTable *next;

	Ink_HashTable *bonding;
	Ink_HashTable *bondee;

	Ink_Object *setter;
	Ink_Object *getter;

	std::string *key_p;

	Ink_HashTable(const char *key, Ink_Object *value, std::string *key_p = NULL);
	Ink_HashTable(const char *key, Ink_InterpreteEngine *engine, Ink_Constant *value, std::string *key_p = NULL);

	Ink_HashTable(Ink_Object *value);

	Ink_HashTable *getEnd();
	// Ink_HashTable *getMapping(const char *key);
	Ink_Object *getValue();
	Ink_Object *setValue(Ink_Object *val);
	Ink_Object *setValue(Ink_InterpreteEngine *engine, Ink_Constant *val);

	inline bool isConstant()
	{
		return type == HASH_CONST;
	}

	inline void initValue()
	{
		const_value.value = NULL;
		const_value.engine = NULL;
		return;
	}

	inline void cleanConst()
	{
		if (type == HASH_CONST)
			delete const_value.value;
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

	~Ink_HashTable();
};

}

#endif
