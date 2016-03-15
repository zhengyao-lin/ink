#ifndef _MOD_JSON_H_
#define _MOD_JSON_H_

#include "core/object.h"
#include "core/error.h"
#include "core/interface/engine.h"

using namespace ink;

Ink_Object *InkNative_JSON_Encode(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);
Ink_Object *InkNative_JSON_Decode(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);

extern Ink_ModuleID ink_native_json_mod_id;

enum InkMod_JSON_ExceptionCode {
	INK_EXCODE_WARN_JSON_CYCLIC_REFERENCE = INK_EXCODE_CUSTOM_START
};

inline void
InkWarn_JSON_Cyclic_Reference(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_json_mod_id,
						   INK_EXCODE_WARN_JSON_CYCLIC_REFERENCE,
						   "Cyclic object reference");
	return;
}

#endif
