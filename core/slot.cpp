#include <string.h>
#include "hash.h"
#include "object.h"
#include "interface/engine.h"
#include "native/native.h"

namespace ink {

using namespace std;

extern int numeric_native_method_table_count;
extern InkNative_MethodTable numeric_native_method_table[];
extern int string_native_method_table_count;
extern InkNative_MethodTable string_native_method_table[];
extern int object_native_method_table_count;
extern InkNative_MethodTable object_native_method_table[];
extern int array_native_method_table_count;
extern InkNative_MethodTable array_native_method_table[];
extern int function_native_method_table_count;
extern InkNative_MethodTable function_native_method_table[];
extern int big_num_native_method_table_count;
extern InkNative_MethodTable big_num_native_method_table[];

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

Ink_HashTable *Ink_Object::getSlotMapping(Ink_InterpreteEngine *engine, const char *key, bool *is_from_proto)
{
	Ink_HashTable *i;
	Ink_HashTable *ret = NULL;
	Ink_Object *proto = getProto();

#if 1
	if (!strcmp(key, "prototype")) {
		for (ret = proto_hash; ret->bonding; ret = ret->bonding) ;
		ret->bondee = proto_hash;
		if (is_from_proto) *is_from_proto = false;
		return ret;
	}
#endif

	for (i = hash_table; i; i = i->next) {
		if (!strcmp(i->key, key) && (i->getValue() || i->bonding)) {
			for (ret = i; ret->bonding; ret = ret->bonding) ;
			ret->bondee = i;
			if (is_from_proto) *is_from_proto = false;
			return ret;
		}
	}

	/* base pointer has to be local -- it cannot be achieved from prototype */
	if (!strcmp(key, "base")) {
		return ret;
	}

	if (engine && proto) {
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

#if 0
	if (!engine) return ret;

	ret = engine->searchNativeMethod(type, key);
	if (ret) {
		ret = setSlot(ret->key, ret->getValue()->clone(engine));
	}
#endif

#if 0
	Ink_Object *p = NULL;

	if ((p = getProto()) != NULL) {
		ret = p->getSlotMapping(engine, key);
		if (ret) {
			ret = setSlot(ret->key, ret->getValue());
		}
	}
#endif

	engine->initPrototypeSearch();
	return ret;
}

Ink_HashTable *Ink_Object::setSlot(const char *key, Ink_Object *value, bool if_check_exist, string *key_p)
{
	Ink_HashTable *i, *slot = NULL, *bond_tracer, *last = NULL;

	if (!strcmp(key, "prototype")) {
		setProto(value);
		if (key_p) delete key_p;
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
		if (key_p) delete key_p;
	} else {
		slot = new Ink_HashTable(key, value, key_p);
		if (hash_table)
			last->next = slot;
		else
			hash_table = slot;
	}

	return slot;
}

void Ink_Object::deleteSlot(const char *key)
{
	Ink_HashTable *i, *prev;

	for (i = hash_table, prev = NULL; i; prev = i, i = i->next) {
		if (!strcmp(i->key, key)) {
			if (prev) {
				prev->next = i->next;
			} else {
				hash_table = i->next;
			}
			delete i;
			return;
		}
	}

	return;
}

void Ink_Object::cleanHashTable()
{
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
