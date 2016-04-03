#include <string.h>
#include "object.h"
#include "interface/engine.h"
#include "native/native.h"

namespace ink {

Ink_Object *Ink_ExpListObject::call(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
									Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv,
									Ink_Object *this_p, bool if_return_this)
{
	Ink_ExpressionList::size_type i;
	Ink_Object *ret = NULL_OBJ;

	for (i = 0; i < exp_list.size(); i++) {
		ret = exp_list[i]->eval(engine, context);
		if (engine->getSignal() != INTER_NONE) {
			return engine->getInterruptValue();
		}
	}

	return ret;
}

}
