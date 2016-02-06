#include <string.h>
#include "hash.h"
#include "object.h"
#include "interface/engine.h"
#include "native/native.h"

namespace ink {

using namespace std;

void Ink_Object::initProto(Ink_InterpreteEngine *engine)
{
	if (engine)
		setProto(engine->getTypePrototype(type));
	return;
}

}
