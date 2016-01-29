#include "hash.h"
#include "object.h"
#include "interface/engine.h"

namespace ink {

using namespace std;

void Ink_Object::cloneHashTable(Ink_Object *src, Ink_Object *dest)
{
	Ink_HashTable *i;
	for (i = src->hash_table; i; i = i->next) {
		if (i->getValue())
			dest->setSlot(i->key, i->getValue(), false);
	}

	return;
}

void Ink_Object::cloneDeepHashTable(Ink_InterpreteEngine *engine, Ink_Object *src, Ink_Object *dest)
{
	Ink_HashTable *i;
	for (i = src->hash_table; i; i = i->next) {
		if (i->getValue()
			&& !engine->cloneDeepHasTraced(i->getValue()))
			dest->setSlot(i->key, i->getValue()->cloneDeep(engine), false);
	}

	return;
}

Ink_Object *Ink_Object::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_Object(engine);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_Object::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_Object *new_obj = new Ink_Object(engine);

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_Object *Ink_ContextObject::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_ContextObject(engine);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_ContextObject::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_Object *new_obj = new Ink_ContextObject(engine);

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_Object *Ink_Numeric::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_Numeric(engine, value);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_Numeric::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_Object *new_obj = new Ink_Numeric(engine, value);

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_Object *Ink_BigNumeric::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_BigNumeric(engine, value);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_BigNumeric::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_Object *new_obj = new Ink_BigNumeric(engine, value);

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_Object *Ink_String::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_String(engine, getValue());

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_String::cloneDeep(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_String(engine, getValue());

	engine->addDeepCloneTrace(this);
	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_ArrayValue Ink_Array::cloneArrayValue(Ink_ArrayValue val)
{
	Ink_ArrayValue ret = Ink_ArrayValue();
	Ink_ArrayValue::size_type i;

	for (i = 0; i < val.size(); i++) {
		if (val[i])
			ret.push_back(new Ink_HashTable("", val[i]->getValue()));
		else ret.push_back(NULL);
	}

	return ret;
}

Ink_ArrayValue Ink_Array::cloneDeepArrayValue(Ink_InterpreteEngine *engine, Ink_ArrayValue val)
{
	Ink_ArrayValue ret = Ink_ArrayValue();
	Ink_ArrayValue::size_type i;

	for (i = 0; i < val.size(); i++) {
		if (val[i]
			&& !engine->cloneDeepHasTraced(val[i]->getValue())) {
			ret.push_back(new Ink_HashTable("", val[i]->getValue()->cloneDeep(engine)));
		}
		else ret.push_back(NULL);
	}

	return ret;
}

Ink_Object *Ink_Array::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_Array(engine, cloneArrayValue(value));

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_Array::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_Object *new_obj = new Ink_Array(engine, cloneDeepArrayValue(engine, value));

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_Object *Ink_Undefined::cloneDeep(Ink_InterpreteEngine *engine)
{
	return UNDEFINED;
}

Ink_Object *Ink_NullObject::cloneDeep(Ink_InterpreteEngine *engine)
{
	return NULL_OBJ;
}

Ink_Object *Ink_Unknown::cloneDeep(Ink_InterpreteEngine *engine)
{
	return new Ink_Unknown(engine);
}

}
