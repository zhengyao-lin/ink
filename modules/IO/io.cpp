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
	Ink_Object *global_context = context->getGlobal()->context;

	if (argc) {
		argv[0]->getSlot(engine, "file")->getSlot(engine, "load")->call(engine, context);
		argv[0]->getSlot(engine, "direct")->getSlot(engine, "load")->call(engine, context);
	}
	InkMod_IO_bondTo(engine, global_context);

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
