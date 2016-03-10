#include "hash.h"
#include "object.h"

namespace ink {

using namespace std;

Ink_HashTable::Ink_HashTable(const char *k, Ink_Object *val, string *key_p)
: value(val), key(k), key_p(key_p)
{
	next = NULL;

	setter = NULL;
	getter = NULL;
	if (val) val->setDebugName(k);
}

Ink_HashTable::Ink_HashTable(Ink_Object *val)
: value(val), key(""), key_p(NULL)
{
	next = NULL;

	setter = NULL;
	getter = NULL;
	if (val) val->setDebugName(key);
}

Ink_HashTable *Ink_HashTable::getEnd()
{
	Ink_HashTable *i;
	for (i = this; i && i->next; i = i->next) ;
	return i;
}

Ink_Object *Ink_HashTable::setValue(Ink_Object *val)
{
	value = val;
	if (val) val->setDebugName(key);
	return val;
}

Ink_HashTable::~Ink_HashTable()
{
	// if (value) value->address = NULL;
	if (key_p) delete key_p;
}

}
