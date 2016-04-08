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

	u.value = val;
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
		IGC_CHECK_WRITE_BARRIER(p, val);
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
	u.const_value.value = val;
	u.const_value.engine = engine;

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

	u.value = val;
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
		IGC_CHECK_WRITE_BARRIER(p, val);
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
		return u.value;
	else {
		assert(u.const_value.engine || !u.const_value.value);
		return u.const_value.value
			   ? u.const_value.value->toObject(u.const_value.engine)
			   : NULL;
	}
}

Ink_Object *Ink_HashTable::setValue(Ink_Object *val)
{
	Ink_InterpreteEngine *engine;
	Ink_Object *p;

	if (type != HASH_CONST) {
		u.value = val;
		if (val) {
			val->setDebugName(key);
			type = HASH_OBJ;
			if ((p = getParent()) != NULL) {
				engine = p->engine;
				IGC_CHECK_WRITE_BARRIER(p, val);
			}
		}
	} else {
		if (val) {
			if (u.const_value.value) {
				InkWarn_Assign_Fixed(u.const_value.engine, key);
			} else {
				cleanConst();
				u.const_value.value = val->toConstant(val->engine);
				u.const_value.engine = val->engine;
				if (!u.const_value.value) {
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
		u.const_value.value = val;
		u.const_value.engine = engine;
	} else {
		setUndefined();
	}

	return NULL;
}

void Ink_HashTable::setConstant()
{
	if (type == HASH_OBJ) {
		if (u.value) {
			u.const_value.value = u.value->toConstant(u.const_value.engine = u.value->engine);
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
		IGC_CHECK_WRITE_BARRIER(p, obj);
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
		IGC_CHECK_WRITE_BARRIER(p, obj);
	}

	return;
}

Ink_Object *Ink_HashTable::getGetter()
{
	return getter;
}

void Ink_HashTable::setBonding(Ink_InterpreteEngine *engine, Ink_HashTable *to, bool if_remove)
{
	bonding = to;
	if (to)
		engine->addGCBonding(this, to);
	else if (if_remove)
		engine->removeGCBonding(this);
	return;
}

Ink_HashTable::~Ink_HashTable()
{
	// if (u.value) u.value->address = NULL;
	if (key_p) delete key_p;
	if (isConstant()) delete u.const_value.value;
}

}
