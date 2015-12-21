#include "hash.h"
#include "object.h"

Ink_HashTable::Ink_HashTable(const char *key, Ink_Object *value)
: value(value), key(key)
{
	next = NULL;
	bonding = NULL;
	bondee = NULL;

	setter = NULL;
	getter = NULL;

	if (value) value->ref_count++;
}

Ink_HashTable::Ink_HashTable(Ink_Object *value)
: value(value), key("")
{
	next = NULL;
	bonding = NULL;
	bondee = NULL;

	setter = NULL;
	getter = NULL;

	if (value) value->ref_count++;
}

Ink_HashTable *Ink_HashTable::getEnd()
{
	Ink_HashTable *i;
	for (i = this; i && i->next; i = i->next) ;
	return i;
}

Ink_Object *Ink_HashTable::setValue(Ink_Object *val) {
	if (value) value->ref_count--;
	value = val;
	return val;
}

/* Ink_HashTable *Ink_HashTable::getMapping(const char *key)
{
	Ink_HashTable *i;
	for (i = this; i; i = i->next) {
		if (!strcmp(i->key, key)) {
			return i;
		}
	}
	return NULL;
}

Ink_Object *Ink_HashTable::getValue(const char *key)
{
	Ink_HashTable *mapping = getMapping(key);
	return mapping ? mapping->value : NULL;
} */