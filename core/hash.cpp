#include "hash.h"
#include "object.h"
#include "constant.h"

namespace ink {

using namespace std;

Ink_HashTable::Ink_HashTable(const char *k, Ink_Object *val, string *k_p)
{
	initValue();

	value = val;
	key = k;
	key_p = k_p;

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
{
	initValue();

	value = val;
	key = "";
	key_p = NULL;

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
		assert(const_value.engine || !const_value.value);
		return const_value.value
			   ? const_value.value->toObject(const_value.engine)
			   : NULL;
	}
}

Ink_Object *Ink_HashTable::setValue(Ink_Object *val)
{
	if (type != HASH_CONST) {
		value = val;
		if (val) {
			val->setDebugName(key);
			type = HASH_OBJ;
		}
	} else {
		if (val) {
			if (const_value.value) {
				InkWarn_Assign_Fixed(const_value.engine, key);
			} else {
				cleanConst();
				const_value.value = val->toConstant(val->engine);
				const_value.engine = val->engine;
				if (!const_value.value) {
					InkWarn_Failed_Get_Constant(val->engine, val->type);
				}
			}
		} else {
			setUndefined();
		}
	}

	return val;
}

void Ink_HashTable::setConstant()
{
	if (type == HASH_OBJ) {
		if (value) {
			const_value.value = value->toConstant(const_value.engine = value->engine);
		}
	}
	type = HASH_CONST;

	return;
}

Ink_HashTable::~Ink_HashTable()
{
	// if (value) value->address = NULL;
	if (key_p) delete key_p;
	if (isConstant()) delete const_value.value;
}

}
