#ifndef _MOD_BLUEPRINT_H_
#define _MOD_BLUEPRINT_H_

#include "core/object.h"
#include "core/general.h"
#include "core/interface/engine.h"
#include "core/package/load.h"

using namespace ink;

enum InkMod_Blueprint_Math_CalMode {
	CALMODE_RADIAN,
	CALMODE_DEGREE
};

class InkMod_Blueprint_EngineCom {
public:
	InkMod_Blueprint_Math_CalMode math_cal_mode;

	InkMod_Blueprint_EngineCom()
	: math_cal_mode(CALMODE_RADIAN)
	{ }
};

void setMathCalMode(Ink_InterpreteEngine *engine, InkMod_Blueprint_Math_CalMode mode);
InkMod_Blueprint_Math_CalMode getMathCalMode(Ink_InterpreteEngine *engine);

/* blueprint.base */
Ink_Object *InkMod_Blueprint_Base_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);
void InkMod_Blueprint_Base_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee);

/* blueprint.sys */
Ink_Object *InkMod_Blueprint_System_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);
void InkMod_Blueprint_System_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee);

/* blueprint.math */
Ink_Object *InkMod_Blueprint_Math_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);
void InkMod_Blueprint_Math_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee);

#endif
