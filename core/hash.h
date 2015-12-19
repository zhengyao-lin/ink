#ifndef _HASH_H_
#define _HASH_H_

#include <stdio.h>

class Ink_Object;

class Ink_HashTable {
public:
	const char *key;
	Ink_Object *value;

	Ink_HashTable *next;
	Ink_HashTable *bonding;
	Ink_HashTable *bondee;

	Ink_Object *setter;
	Ink_Object *getter;

	Ink_HashTable(const char *key, Ink_Object *value)
	: key(key), value(value)
	{
		next = NULL;
		bonding = NULL;
		bondee = NULL;

		setter = NULL;
		getter = NULL;
	}

	Ink_HashTable(Ink_Object *value)
	: key(""), value(value)
	{
		next = NULL;
		bonding = NULL;
		bondee = NULL;

		setter = NULL;
		getter = NULL;
	}

	Ink_HashTable *getEnd();
	Ink_HashTable *getMapping(const char *key);
	Ink_Object *getValue(const char *key);
};

#endif