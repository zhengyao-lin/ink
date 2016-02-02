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

InkMod_ModuleID ink_native_file_mod_id;

struct com_cleaner_arg {
	InkMod_ModuleID id;
	com_cleaner_arg(InkMod_ModuleID id)
	: id(id)
	{ }
};

void InkNative_IO_EngineComCleaner(Ink_InterpreteEngine *engine, void *arg)
{
	com_cleaner_arg *tmp = (com_cleaner_arg *)arg;
	free(engine->getEngineComAs<Ink_TypeTag>(tmp->id));
	delete tmp;
	return;
}

void InkMod_IO_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot("stdin", new Ink_FilePointer(engine, stdin));
	bondee->setSlot("stdout", new Ink_FilePointer(engine, stdout));
	bondee->setSlot("stderr", new Ink_FilePointer(engine, stderr));

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
	}

	tmp_argv[0] = direct_pkg;

	if (direct_loader->type == INK_FUNCTION) {
		direct_loader->call(engine, context, 2, tmp_argv);
	} else {
		InkWarn_Package_Broken(engine, "io.direct");
	}

	free(tmp_argv);

	InkMod_IO_bondTo(engine, apply_to);

	return NULL_OBJ;
}

extern "C" {
	void InkMod_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
	{
		Ink_Object *tmp;
		Ink_TypeTag *type_p = NULL;

		if (!engine->getEngineComAs<Ink_TypeTag>(ink_native_file_mod_id)) {
			type_p = (Ink_TypeTag *)malloc(sizeof(Ink_TypeTag) * 2);

			type_p[0] = (Ink_TypeTag)engine->registerType("io.file.File");
			type_p[1] = (Ink_TypeTag)engine->registerType("io.direct.Directory");

			engine->addEngineCom(ink_native_file_mod_id, type_p);
			engine->addDestructor(Ink_EngineDestructor(InkNative_IO_EngineComCleaner, new com_cleaner_arg(ink_native_file_mod_id)));

			context->getGlobal()->context->setSlot("$io.file.File", tmp = new Ink_FilePointer(engine));
			tmp->derivedMethodInit(engine);
			context->getGlobal()->context->setSlot("$io.direct.Directory", tmp = new Ink_DirectPointer(engine));
			tmp->derivedMethodInit(engine);
		}

		Ink_Object *io_pkg = addPackage(engine, context, "io", new Ink_FunctionObject(engine, InkMod_IO_Loader));
		Ink_Object *io_file_pkg = addPackage(engine, io_pkg, "file", new Ink_FunctionObject(engine, InkMod_File_Loader));
		Ink_Object *io_direct_pkg = addPackage(engine, io_pkg, "direct", new Ink_FunctionObject(engine, InkMod_Direct_Loader));

		InkMod_IO_bondTo(engine, io_pkg);
		InkMod_File_bondTo(engine, io_file_pkg);
		InkMod_Direct_bondTo(engine, io_direct_pkg);
	}

	int InkMod_Init(InkMod_ModuleID id)
	{
		ink_native_file_mod_id = id;
		return 0;
	}
}
