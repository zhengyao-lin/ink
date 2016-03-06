#include <readline/readline.h>
#include <readline/history.h>

#include <string>
#include "core/object.h"
#include "core/general.h"
#include "core/native/native.h"
#include "core/interface/engine.h"

using namespace ink;
using namespace std;

Ink_Object *InkNative_ReadLine_Read(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	string pmt = as<Ink_String>(argv[0])->getValue();
	char *ret_val = readline(pmt.c_str());
	Ink_Object *ret = NULL_OBJ;

	if (!ret_val || !*ret_val) {
		goto CLEAN;
	}

	ret = new Ink_String(engine, string(ret_val));
	add_history(ret_val);

CLEAN:
	if (ret_val) free(ret_val);

	return ret;
}

void InkMod_ReadLine_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot_c("read", new Ink_FunctionObject(engine, InkNative_ReadLine_Read));

	return;
}

Ink_Object *InkMod_ReadLine_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *apply_to = argv[1];
	Ink_Object *readline_pkg = addPackage(engine, apply_to, "readline", new Ink_FunctionObject(engine, InkMod_ReadLine_Loader));

	InkMod_ReadLine_bondTo(engine, readline_pkg);

	return NULL_OBJ;
}

extern "C" {
	static InkMod_ModuleID ink_native_readline_mod_id;

	void InkMod_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
	{
		addPackage(engine, context, "readline", new Ink_FunctionObject(engine, InkMod_ReadLine_Loader));

		return;
	}

	int InkMod_Init(InkMod_ModuleID id)
	{
		ink_native_readline_mod_id = id;
		return 0;
	}
}
