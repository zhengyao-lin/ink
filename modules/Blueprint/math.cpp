#include <math.h>
#include "blueprint.h"
#include "error.h"
#include "core/object.h"
#include "core/native/general.h"
#include "core/gc/collect.h"

#define M_PI 3.14159265358979323846

using namespace ink;

Ink_Object *InkMod_Blueprint_Math_ToDegree(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, 180 / M_PI * as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkMod_Blueprint_Math_ToRadian(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, M_PI / 180 * as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkMod_Blueprint_Math_Sine(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, sin(as<Ink_Numeric>(argv[0])->value));
}

Ink_Object *InkMod_Blueprint_Math_Cosine(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, cos(as<Ink_Numeric>(argv[0])->value));
}

Ink_Object *InkMod_Blueprint_Math_Tangent(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, tan(as<Ink_Numeric>(argv[0])->value));
}

void InkMod_Blueprint_Math_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot_c("deg", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_ToDegree));
	bondee->setSlot_c("rad", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_ToRadian));

	bondee->setSlot_c("sin", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Sine));
	bondee->setSlot_c("cos", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Cosine));
	bondee->setSlot_c("tan", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Tangent));

	bondee->setSlot_c("pi", new Ink_Numeric(engine, M_PI));

	return;
}

Ink_Object *InkMod_Blueprint_Math_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *apply_to = argv[1];

	InkMod_Blueprint_Math_bondTo(engine, apply_to);

	return NULL_OBJ;
}