#ifndef _BLUEPRINT_H_
#define _BLUEPRINT_H_

#include "core/object.h"
#include "core/general.h"
#include "core/interface/engine.h"
#include "core/package/load.h"

using namespace ink;

/* blueprint.base */
Ink_Object *InkMod_Blueprint_Base_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);
void InkMod_Blueprint_Base_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee);

/* blueprint.sys */
Ink_Object *InkMod_Blueprint_System_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);
void InkMod_Blueprint_System_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee);

#endif
