#include <math.h>
#include "blueprint.h"
#include "error.h"
#include "core/object.h"
#include "core/native/general.h"
#include "core/gc/collect.h"

#define INK_M_PI 3.14159265358979323846
#define INK_M_E 2.71828182845904523536

using namespace ink;

Ink_Object *InkMod_Blueprint_Math_SetMode(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	string mode_str = as<Ink_String>(argv[0])->getValue();
	InkMod_Blueprint_Math_CalMode mode_e;

	if (mode_str == "rad" || mode_str == "radian") {
		mode_e = CALMODE_RADIAN;
	} else if (mode_str == "deg" || mode_str == "degree") {
		mode_e = CALMODE_DEGREE;
	} else {
		InkWarn_Blueprint_Math_Set_Unknown_Mode(engine, mode_str.c_str());
		return NULL_OBJ;
	}

	setMathCalMode(engine, mode_e);

	return new Ink_Numeric(engine, true);
}

Ink_Object *InkMod_Blueprint_Math_GetMode(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	string ret_str = "unknown";
	InkMod_Blueprint_Math_CalMode mode_e = getMathCalMode(engine);
	bool is_abbr = false;

	if (argc && argv[0]->type == INK_NUMERIC && as<Ink_Numeric>(argv[0])->value) {
		is_abbr = true;
	}

	switch (mode_e) {
		case CALMODE_RADIAN:
			if (is_abbr) ret_str = "rad";
			else ret_str = "radian";
			break;
		case CALMODE_DEGREE:
			if (is_abbr) ret_str = "deg";
			else ret_str = "degree";
			break;
		default:
			InkWarn_Blueprint_Math_Get_Unknown_Mode(engine, mode_e);
			return NULL_OBJ;
	}

	return new Ink_String(engine, ret_str);
}

inline double
castValueByMode(Ink_InterpreteEngine *engine, double val /* output radian */)
{
	switch (getMathCalMode(engine)) {
		case CALMODE_RADIAN:
			return val;
		case CALMODE_DEGREE:
			return INK_M_PI / 180 * val;
	}

	return val;
}

inline double
_getOutputValue(Ink_InterpreteEngine *engine, double val /* input value */)
{
	switch (getMathCalMode(engine)) {
		case CALMODE_RADIAN:
			return val;
		case CALMODE_DEGREE:
			return 180 / INK_M_PI * val;
	}

	return val;
}

inline double
_getInputValue(Ink_InterpreteEngine *engine, Ink_Object *num /* assume numeric */)
{
	return castValueByMode(engine, as<Ink_Numeric>(num)->value);
}

#define getInputValue(num) (_getInputValue(engine, (num)))
#define getOutputValue(num) (_getOutputValue(engine, (num)))

Ink_Object *InkMod_Blueprint_Math_ToDegree(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, 180 / INK_M_PI * as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkMod_Blueprint_Math_ToRadian(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, INK_M_PI / 180 * as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkMod_Blueprint_Math_Sine(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, sin(getInputValue(argv[0])));
}

Ink_Object *InkMod_Blueprint_Math_Cosine(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, cos(getInputValue(argv[0])));
}

Ink_Object *InkMod_Blueprint_Math_Tangent(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, tan(getInputValue(argv[0])));
}

Ink_Object *InkMod_Blueprint_Math_ASine(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, getOutputValue(asin(as<Ink_Numeric>(argv[0])->value)));
}

Ink_Object *InkMod_Blueprint_Math_ACosine(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, getOutputValue(acos(as<Ink_Numeric>(argv[0])->value)));
}

Ink_Object *InkMod_Blueprint_Math_ATangent(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	if (argc > 1 && argv[1]->type == INK_NUMERIC) {
		return new Ink_Numeric(engine, getOutputValue(atan2(as<Ink_Numeric>(argv[0])->value,
															as<Ink_Numeric>(argv[1])->value)));
	}

	return new Ink_Numeric(engine, getOutputValue(atan(as<Ink_Numeric>(argv[0])->value)));
}

Ink_Object *InkMod_Blueprint_Math_SineH(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, sinh(getInputValue(argv[0])));
}

Ink_Object *InkMod_Blueprint_Math_CosineH(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, cosh(getInputValue(argv[0])));
}

Ink_Object *InkMod_Blueprint_Math_TangentH(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, tanh(getInputValue(argv[0])));
}

Ink_Object *InkMod_Blueprint_Math_Hypotenuse(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 2, INK_NUMERIC, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, hypot(as<Ink_Numeric>(argv[0])->value,
										 as<Ink_Numeric>(argv[1])->value));
}

Ink_Object *InkMod_Blueprint_Math_SquareRoot(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, sqrt(as<Ink_Numeric>(argv[0])->value));
}

Ink_Object *InkMod_Blueprint_Math_Pow(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 2, INK_NUMERIC, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, pow(as<Ink_Numeric>(argv[0])->value,
									   as<Ink_Numeric>(argv[1])->value));
}

Ink_Object *InkMod_Blueprint_Math_Exp(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, exp(as<Ink_Numeric>(argv[0])->value));
}

Ink_Object *InkMod_Blueprint_Math_Log(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, log(as<Ink_Numeric>(argv[0])->value));
}

Ink_Object *InkMod_Blueprint_Math_Log10(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, log10(as<Ink_Numeric>(argv[0])->value));
}

void InkMod_Blueprint_Math_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot_c("setmode", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_SetMode));
	bondee->setSlot_c("getmode", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_GetMode));

	bondee->setSlot_c("to_deg", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_ToDegree));
	bondee->setSlot_c("to_rad", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_ToRadian));

	bondee->setSlot_c("sin", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Sine));
	bondee->setSlot_c("cos", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Cosine));
	bondee->setSlot_c("tan", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Tangent));

	bondee->setSlot_c("asin", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_ASine));
	bondee->setSlot_c("acos", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_ACosine));
	bondee->setSlot_c("atan", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_ATangent));

	bondee->setSlot_c("sinh", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_SineH));
	bondee->setSlot_c("cosh", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_CosineH));
	bondee->setSlot_c("tanh", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_TangentH));

	bondee->setSlot_c("hypot", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Hypotenuse));

	bondee->setSlot_c("sqrt", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_SquareRoot));
	bondee->setSlot_c("pow", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Pow));
	bondee->setSlot_c("exp", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Exp));
	bondee->setSlot_c("log", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Log));
	bondee->setSlot_c("log10", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Log10));

	bondee->setSlot_c("pi", new Ink_Numeric(engine, INK_M_PI));
	bondee->setSlot_c("e", new Ink_Numeric(engine, INK_M_E));

	return;
}

Ink_Object *InkMod_Blueprint_Math_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *apply_to = argv[1];
	Ink_Object *math_pkg = addPackage(engine, apply_to, "math", new Ink_FunctionObject(engine, InkMod_Blueprint_Math_Loader));

	InkMod_Blueprint_Math_bondTo(engine, math_pkg);

	return NULL_OBJ;
}