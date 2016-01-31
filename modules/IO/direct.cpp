#include <stdio.h>
#include <string>
#include "core/general.h"
#include "core/debug.h"
#include "core/native/native.h"
#include "core/interface/engine.h"
#include "includes/switches.h"
#include "io.h"

using namespace ink;
using namespace std;

extern InkMod_ModuleID ink_native_file_mod_id;

Ink_TypeTag getDirectType(Ink_InterpreteEngine *engine)
{
	return engine->getEngineComAs<Ink_TypeTag>(ink_native_file_mod_id)[1];
}

Ink_Object *InkNative_Direct_Constructor(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *ret;

	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	context->getLocal()
	->context->setSlot("this", ret = new Ink_DirectPointer(engine, as<Ink_String>(argv[0])->getValue()));

	return ret;
}

Ink_Object *InkNative_Direct_Exist(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	string *tmp_path;

	ASSUME_BASE_TYPE(engine, DIRECT_TYPE);

	if (!(tmp_path = as<Ink_DirectPointer>(base)->path)) {
		InkWarn_IO_Uninitialized_Direct_Pointer(engine);
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, isDirExist(tmp_path->c_str()));
}

void Ink_DirectPointer::Ink_DirectMethodInit(Ink_InterpreteEngine *engine)
{
	setSlot("exist", new Ink_FunctionObject(engine, InkNative_Direct_Exist));

	return;
}

void InkMod_Direct_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot("Directory", new Ink_FunctionObject(engine, InkNative_Direct_Constructor));

	return;
}

Ink_Object *InkMod_Direct_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *global_context = context->getGlobal()->context;
	
	InkMod_Direct_bondTo(engine, global_context);

	return NULL_OBJ;
}