#include "hash.h"
#include "object.h"
#include "constant.h"

namespace ink {

using namespace std;

Ink_HashTable::Ink_HashTable(const char *k, Ink_Object *val, string *key_p)
: value(val), key(k), key_p(key_p)
{
	next = NULL;
	bonding = NULL;
	bondee = NULL;

	setter = NULL;
	getter = NULL;
	if (val) {
		val->setDebugName(k);
		type = HASH_OBJ;
	} else {
		type = HASH_UNDEFINED;
	}
}

Ink_HashTable::Ink_HashTable(Ink_Object *val)
: value(val), key(""), key_p(NULL)
{
	next = NULL;
	bonding = NULL;
	bondee = NULL;

	setter = NULL;
	getter = NULL;
	if (val) {
		val->setDebugName(key);
		type = HASH_OBJ;
	} else {
		type = HASH_UNDEFINED;
	}
}

Ink_HashTable *Ink_HashTable::getEnd()
{
	Ink_HashTable *i;
	for (i = this; i && i->next; i = i->next) ;
	return i;
}

Ink_Object *Ink_HashTable::getValue()
{
	if (type != HASH_CONST)
		return value;
	else {
		assert(const_value.engine);
		return const_value.value
			   ? const_value.value->toObject(const_value.engine)
			   : NULL;
	}
}

Ink_Object *Ink_HashTable::setValue(Ink_Object *val)
{
	if (type != HASH_CONST) {
		value = val;
		if (val) val->setDebugName(key);
	} else {
		const_value.value = val ? val->toConstant(NULL) : NULL;
		const_value.engine = val ? val->engine : NULL;
	}

	return val;
}

Ink_HashTable::~Ink_HashTable()
{
	// if (value) value->address = NULL;
	if (key_p) delete key_p;
}

}
