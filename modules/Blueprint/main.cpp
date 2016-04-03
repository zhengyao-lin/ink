#include <string.h>
#include "blueprint.h"
#include "core/object.h"
#include "core/native/general.h"

using namespace ink;

Ink_ModuleID ink_native_blueprint_mod_id;

void setMathCalMode(Ink_InterpreteEngine *engine, InkMod_Blueprint_Math_CalMode mode)
{
	engine->getEngineComAs<InkMod_Blueprint_EngineCom>(ink_native_blueprint_mod_id)->math_cal_mode = mode;
	return;
}

InkMod_Blueprint_Math_CalMode getMathCalMode(Ink_InterpreteEngine *engine)
{
	return engine->getEngineComAs<InkMod_Blueprint_EngineCom>(ink_native_blueprint_mod_id)->math_cal_mode;
}

Ink_Object *InkMod_Blueprint_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *self = argv[0];
	Ink_Object *apply_to = argv[1];
	Ink_Object **tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *) * 2);

	/* blueprint.base */
	Ink_Object *base_pkg = self->getSlot(engine, "base");
	Ink_Object *base_loader = base_pkg->getSlot(engine, "load");

	tmp_argv[0] = base_pkg;
	tmp_argv[1] = apply_to;

	if (base_loader->type == INK_FUNCTION) {
		base_loader->call(engine, context, base_pkg, 2, tmp_argv);
	} else {
		InkWarn_Package_Broken(engine, "blueprint.base");
	}

	/*
	apply_to->setSlot_c("sys", self->getSlot(engine, "sys"));
	apply_to->setSlot_c("math", self->getSlot(engine, "math"));
	*/

	free(tmp_argv);

	return NULL_OBJ;
}

struct com_cleaner_arg {
	Ink_ModuleID id;
	com_cleaner_arg(Ink_ModuleID id)
	: id(id)
	{ }
};

void InkMod_Blueprint_EngineComCleaner(Ink_InterpreteEngine *engine, void *arg)
{
	com_cleaner_arg *tmp = (com_cleaner_arg *)arg;
	delete engine->getEngineComAs<InkMod_Blueprint_EngineCom>(tmp->id);
	delete tmp;
	return;
}

extern "C" {
	void InkMod_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
	{
		Ink_Object *blueprint_pkg = addPackage(engine, context, "blueprint",
											   new Ink_FunctionObject(engine, InkMod_Blueprint_Loader));

		addPackage(engine, blueprint_pkg, "base", new Ink_FunctionObject(engine, InkMod_Blueprint_Base_Loader));
		addPackage(engine, blueprint_pkg, "sys", new Ink_FunctionObject(engine, InkMod_Blueprint_System_Loader));
		addPackage(engine, blueprint_pkg, "math", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Loader));
		addPackage(engine, blueprint_pkg, "time", new Ink_FunctionObject(engine, InkMod_Blueprint_Time_Loader));

		return;
	}

	int InkMod_Init(Ink_ModuleID id)
	{
		ink_native_blueprint_mod_id = id;
		return 0;
	}
}