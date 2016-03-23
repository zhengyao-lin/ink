#include <math.h>
#include "blueprint.h"
#include "error.h"
#include "core/object.h"
#include "core/time.h"
#include "core/native/general.h"

Ink_Object *InkMod_Blueprint_Time_MSleep(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	Ink_NumericValue val = as<Ink_Numeric>(argv[0])->getValue();
	Ink_msleep(getInt(val));

	return NULL_OBJ;
}

void InkMod_Blueprint_Time_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot_c("msleep", new Ink_FunctionObject(engine, InkMod_Blueprint_Time_MSleep));
	return;
}

Ink_Object *InkMod_Blueprint_Time_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *apply_to = argv[1];
	Ink_Object *time_pkg = addPackage(engine, apply_to, "time", new Ink_FunctionObject(engine, InkMod_Blueprint_Time_Loader));

	InkMod_Blueprint_Time_bondTo(engine, time_pkg);

	return NULL_OBJ;
}