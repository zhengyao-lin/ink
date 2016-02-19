#include <string.h>
#include "hash.h"
#include "object.h"
#include "interface/engine.h"
#include "native/native.h"

namespace ink {

using namespace std;

Ink_Object *getMethod(Ink_InterpreteEngine *engine,
					  Ink_Object *obj, const char *name, InkNative_MethodTable *table, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		if (!strcmp(name, table[i].name)) {
			return table[i].func->clone(engine);
		}
	}
	return NULL;
}

Ink_Object *Ink_Object::getSlot(Ink_InterpreteEngine *engine, const char *key)
{
	Ink_HashTable *ret = getSlotMapping(engine, key);

	return ret ? ret->getValue() : UNDEFINED;
}

Ink_Object *Ink_Object::getProto()
{
	Ink_HashTable *ret = getProtoHash(false);

	return ret ? ret->getValue() : NULL;
}

Ink_HashTable *Ink_Object::getSlotMapping(Ink_InterpreteEngine *engine, const char *key, bool *is_from_proto)
{
	Ink_HashTable *i;
	Ink_HashTable *ret = NULL;
	Ink_Object *proto = getProto();

#if 1
	if (!strcmp(key, "prototype")) {
		ret = getProtoHash();
		if (is_from_proto) *is_from_proto = false;
		return ret && ret->getValue() ? ret : NULL;
	}
#endif

	for (i = hash_table; i; i = i->next) {
		if (!strcmp(i->key, key)) {
			if (i->setter || i->getter) {
				if (is_from_proto) *is_from_proto = false;
				return i;
			} else if(i->getValue() || i->bonding) {
				ret = traceHashBond(i);
				if (is_from_proto) *is_from_proto = false;
				return ret;
			}
		}
	}

	/* base pointer has to be local -- it cannot be achieved from prototype */
#if 0
	if (!strcmp(key, "base")) {
		return ret;
	}
#endif

	if (engine && proto && proto->type != INK_UNDEFINED) {
		if (engine->prototypeHasTraced(this)) {
			InkWarn_Circular_Prototype_Reference(engine);
			engine->initPrototypeSearch();
			return NULL;
		}
		engine->addPrototypeTrace(this);

		ret = proto->getSlotMapping(engine, key);
		
		if (ret) {
			if (is_from_proto) *is_from_proto = true;
			engine->initPrototypeSearch();
			return ret;
		}
	}

	engine->initPrototypeSearch();
	return ret;
}

Ink_HashTable *Ink_Object::setSlot(const char *key, Ink_Object *value, bool if_check_exist, bool if_alloc_key)
{
	Ink_HashTable *i, *slot = NULL, *bond_tracer, *last = NULL;

	if (!strcmp(key, "prototype")) {
		setProto(value);
		return proto_hash;
	}
	
	for (i = hash_table; i; i = i->next) {
		if (if_check_exist) {
			if (!strcmp(i->key, key)) {
				for (bond_tracer = i; bond_tracer->bonding; bond_tracer = bond_tracer->bonding) ;
				bond_tracer->bondee = i;
				slot = bond_tracer;
			}
		}
		last = i;
	}

	if (slot) {
		slot->setValue(value);
	} else {
		if (if_alloc_key) {
			string *key_p = new string(key);
			slot = new Ink_HashTable(key_p->c_str(), value, key_p);
		} else {
			slot = new Ink_HashTable(key, value);
		}
		if (hash_table)
			last->next = slot;
		else
			hash_table = slot;
	}

	return slot;
}

void Ink_Object::deleteSlot(const char *key)
{
	Ink_HashTable *i;

	for (i = hash_table; i; i = i->next) {
		if (!strcmp(i->key, key)) {
			i->setValue(NULL);
			return;
		}
	}

	return;
}

void Ink_Object::cleanHashTable()
{
	Ink_Object *obj;

	if (proto_hash) {
		if ((obj = engine->getGlobalReturnValue()) != NULL
			&& obj->address == proto_hash) {
			obj->address = NULL;
		}
		engine->breakUnreachableBonding(proto_hash);
		delete proto_hash;
		proto_hash = NULL;
	}

	cleanHashTable(hash_table);
	hash_table = NULL;

	return;
}

void Ink_Object::cleanHashTable(Ink_HashTable *table)
{
	Ink_Object *obj;
	Ink_HashTable *i, *tmp;

	for (i = table; i;) {
		tmp = i;
		i = i->next;
		/* if ((obj = tmp->getValue()) != NULL) {
			obj->address = NULL;
		} */
		if ((obj = engine->getGlobalReturnValue()) != NULL
			&& obj->address == tmp) {
			obj->address = NULL;
		}
		engine->breakUnreachableBonding(tmp);
		delete tmp;
	}

	return;
}

}
