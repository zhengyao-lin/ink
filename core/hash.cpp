#include "hash.h"
#include "object.h"
#include "constant.h"
#include "interface/engine.h"

namespace ink {

using namespace std;

Ink_HashTable::Ink_HashTable(const char *k, Ink_Object *val, Ink_Object *p, string *k_p)
{
	Ink_InterpreteEngine *engine;
	
	initValue();

	value = val;
	key = k;
	key_p = k_p;

	next = NULL;
	bonding = NULL;
	bondee = NULL;

	parent = p;

	setter = NULL;
	getter = NULL;

	if (p) {
		engine = p->engine;
		if (IS_WHITE(val) && IS_BLACK(p)) {
			SET_GREY(p);
		}
	}

	if (val) {
		val->setDebugName(k);
		type = HASH_OBJ;
	} else {
		type = HASH_UNDEFINED;
	}
}

Ink_HashTable::Ink_HashTable(const char *k, Ink_InterpreteEngine *engine, Ink_Constant *val, Ink_Object *p, string *k_p)
{
	const_value.value = val;
	const_value.engine = engine;

	key = k;
	key_p = k_p;

	next = NULL;
	bonding = NULL;
	bondee = NULL;

	parent = p;

	setter = NULL;
	getter = NULL;

	if (val) {
		type = HASH_CONST;
	} else {
		type = HASH_UNDEFINED;
	}
}

Ink_HashTable::Ink_HashTable(Ink_Object *val, Ink_Object *p)
{
	Ink_InterpreteEngine *engine;

	initValue();

	value = val;
	key = "";
	key_p = NULL;

	next = NULL;
	bonding = NULL;
	bondee = NULL;

	parent = p;

	setter = NULL;
	getter = NULL;

	if (p) {
		engine = p->engine;
		if (IS_WHITE(val) && IS_BLACK(p)) {
			SET_GREY(p);
		}
	}

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
	Ink_InterpreteEngine *engine;
	Ink_Object *p;

	if (type != HASH_CONST) {
		value = val;
		if (val) {
			val->setDebugName(key);
			type = HASH_OBJ;
			if ((p = getParent()) != NULL) {
				engine = p->engine;
				if (IS_WHITE(val) && IS_BLACK(p)) {
					SET_GREY(p);
				}
			}
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

Ink_Object *Ink_HashTable::setValue(Ink_InterpreteEngine *engine, Ink_Constant *val)
{
	if (type != HASH_CONST) {
		setConstant();
	}

	if (val) {
		/* this function can only be called by native method, so no need to warn */
		cleanConst();
		const_value.value = val;
		const_value.engine = engine;
	} else {
		setUndefined();
	}

	return NULL;
}

void Ink_HashTable::setConstant()
{
	if (type == HASH_OBJ) {
		if (value) {
			const_value.value = value->toConstant(const_value.engine = value->engine);
		}
	}
	
	bondee = NULL;
	bonding = NULL;
	setter = NULL;
	getter = NULL;

	type = HASH_CONST;

	return;
}

void Ink_HashTable::setSetter(Ink_Object *obj)
{
	Ink_InterpreteEngine *engine;
	Ink_Object *p;

	setter = obj;

	if ((p = getParent()) != NULL) {
		engine = p->engine;
		if (IS_WHITE(obj) && IS_BLACK(p)) {
			SET_GREY(p);
		}
	}

	return;
}

Ink_Object *Ink_HashTable::getSetter()
{
	return setter;
}

void Ink_HashTable::setGetter(Ink_Object *obj)
{
	Ink_InterpreteEngine *engine;
	Ink_Object *p;

	getter = obj;
	
	if ((p = getParent()) != NULL) {
		engine = p->engine;
		if (IS_WHITE(obj) && IS_BLACK(p = parent)) {
			SET_GREY(parent);
		}
	}

	return;
}

Ink_Object *Ink_HashTable::getGetter()
{
	return getter;
}

Ink_HashTable::~Ink_HashTable()
{
	// if (value) value->address = NULL;
	if (key_p) delete key_p;
	if (isConstant()) delete const_value.value;
}

}
