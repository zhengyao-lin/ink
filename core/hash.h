#ifndef _HASH_H_
#define _HASH_H_

#include <string>
#include <stdio.h>

class Ink_Object;

class Ink_HashTable {
	Ink_Object *value;
public:
	const char *key;

	Ink_HashTable *next;
	Ink_HashTable *bonding;
	Ink_HashTable *bondee;

	Ink_Object *setter;
	Ink_Object *getter;

	std::string *key_p;

	Ink_HashTable(const char *key, Ink_Object *value, std::string *key_p = NULL);

	Ink_HashTable(Ink_Object *value);

	Ink_HashTable *getEnd();
	// Ink_HashTable *getMapping(const char *key);
	inline Ink_Object *getValue()
	{
		return value;
	}
	Ink_Object *setValue(Ink_Object *val);

	~Ink_HashTable();
};

#endif
