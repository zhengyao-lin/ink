#include <string.h>
#include "blueprint.h"
#include "core/object.h"
#include "core/native/general.h"

using namespace ink;

InkMod_ModuleID ink_native_blueprint_mod_id;

Ink_Object *InkMod_Blueprint_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *self = argv[0];
	Ink_Object *apply_to = argv[1];
	Ink_Object **tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *) * 2);

	Ink_Object *base_pkg = self->getSlot(engine, "base");
	Ink_Object *base_loader = base_pkg->getSlot(engine, "load");

	tmp_argv[0] = base_pkg;
	tmp_argv[1] = apply_to;

	if (base_loader->type == INK_FUNCTION) {
		base_loader->call(engine, context, 2, tmp_argv);
	} else {
		InkWarn_Package_Broken(engine, "blueprint.base");
	}

	free(tmp_argv);

	return NULL_OBJ;
}

extern "C" {
	void InkMod_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
	{
		Ink_Object *blueprint_pkg = addPackage(engine, context, "blueprint",
											   new Ink_FunctionObject(engine, InkMod_Blueprint_Loader));
		Ink_Object *base_pkg = addPackage(engine, blueprint_pkg, "base",
										  new Ink_FunctionObject(engine, InkMod_Blueprint_Base_Loader));

		InkMod_Blueprint_Base_bondTo(engine, base_pkg);
		return;
	}

	int InkMod_Init(InkMod_ModuleID id)
	{
		ink_native_blueprint_mod_id = id;
		return 0;
	}
}