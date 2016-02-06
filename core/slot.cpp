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

Ink_HashTable *Ink_Object::getSlotMapping(Ink_InterpreteEngine *engine, const char *key)
{
	Ink_HashTable *i;
	Ink_HashTable *ret = NULL;

#if 0
	if (!strcmp(key, "prototype")) {
		return proto;
	}
#endif

	for (i = hash_table; i; i = i->next) {
		if (!strcmp(i->key, key) && (i->getValue() || i->bonding)) {
			for (ret = i; ret->bonding; ret = ret->bonding) ;
			ret->bondee = i;
			return ret;
		}
	}
	if (!engine) return ret;

#if 1
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

	return ret;
}

Ink_HashTable *Ink_Object::setSlot(const char *key, Ink_Object *value, bool if_check_exist, string *key_p)
{
	Ink_HashTable *i, *slot = NULL, *bond_tracer, *last = NULL;
	
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
