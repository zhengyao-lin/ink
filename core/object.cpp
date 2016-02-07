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
	if (engine && (proto = engine->getTypePrototype(type))) 
		setProto(proto);
	return;
}

}
