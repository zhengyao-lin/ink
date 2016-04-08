#include <string.h>
#include "hash.h"
#include "object.h"
#include "interface/engine.h"
#include "native/native.h"

namespace ink {

using namespace std;

void Ink_Object::initProto(Ink_InterpreteEngine *engine)
{
	Ink_Object *proto;
	
	if (engine && (proto = engine->getTypePrototype(type))) {
		IGC_CHECK_WRITE_BARRIER(this, proto);
		setProto(proto);
	}

	return;
}

void Ink_Object::setBase(Ink_Object *obj)
{
	base_p = obj;

	IGC_CHECK_WRITE_BARRIER(this, obj);
	
	return;
}

void Ink_Object::setProto(Ink_Object *proto)
{
	// setSlot_c("prototype", proto);
	if (proto_hash) {
		proto_hash->setValue(proto);
	} else {
		proto_hash = new Ink_HashTable("prototype", proto, this);
	}

	IGC_CHECK_WRITE_BARRIER(this, proto);

	return;
}

Ink_Object *Ink_ContextObject::setReturnVal(Ink_Object *obj)
{
	ret_val = obj;
	
	IGC_CHECK_WRITE_BARRIER(this, obj);

	return obj;
}

}
