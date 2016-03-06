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

InkMod_ModuleID ink_native_io_mod_id;

void InkMod_IO_EngineComCleaner(Ink_InterpreteEngine *engine, void *arg)
{
	com_cleaner_arg *tmp = (com_cleaner_arg *)arg;

	delete engine->getEngineComAs<com_struct>(tmp->id);
	delete tmp;
	
	return;
}

void InkMod_IO_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot_c("stdin", new Ink_FilePointer(engine, stdin));
	bondee->setSlot_c("stdout", new Ink_FilePointer(engine, stdout));
	bondee->setSlot_c("stderr", new Ink_FilePointer(engine, stderr));

	return;
}

Ink_Object *InkMod_IO_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *self = argv[0];
	Ink_Object *apply_to = argv[1];
	Ink_Object **tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *) * 2);

	Ink_Object *file_pkg = self->getSlot(engine, "file");
	Ink_Object *direct_pkg = self->getSlot(engine, "direct");

	Ink_Object *file_loader = file_pkg->getSlot(engine, "load");
	Ink_Object *direct_loader = direct_pkg->getSlot(engine, "load");

	tmp_argv[0] = file_pkg;
	tmp_argv[1] = apply_to;

	if (file_loader->type == INK_FUNCTION) {
		file_loader->call(engine, context, 2, tmp_argv);
	} else {
		InkWarn_Package_Broken(engine, "io.file");
		return NULL_OBJ;
	}

	tmp_argv[0] = direct_pkg;

	if (direct_loader->type == INK_FUNCTION) {
		direct_loader->call(engine, context, 2, tmp_argv);
	} else {
		InkWarn_Package_Broken(engine, "io.direct");
		return NULL_OBJ;
	}

	free(tmp_argv);

	InkMod_IO_bondTo(engine, apply_to);

	return NULL_OBJ;
}

extern "C" {
	void InkMod_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
	{
		Ink_Object *io_pkg = addPackage(engine, context, "io", new Ink_FunctionObject(engine, InkMod_IO_Loader));
		addPackage(engine, io_pkg, "file", new Ink_FunctionObject(engine, InkMod_File_Loader));
		addPackage(engine, io_pkg, "direct", new Ink_FunctionObject(engine, InkMod_Direct_Loader));

		return;
	}

	int InkMod_Init(InkMod_ModuleID id)
	{
		ink_native_io_mod_id = id;
		return 0;
	}
}
