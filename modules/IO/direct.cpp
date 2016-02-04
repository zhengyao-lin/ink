#include <stdio.h>
#include <string>
#include "io.h"
#include "core/general.h"
#include "core/debug.h"
#include "core/native/native.h"
#include "core/gc/collect.h"
#include "core/interface/engine.h"
#include "includes/switches.h"

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

Ink_Object *InkNative_Direct_Create(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	string *tmp_path;

	ASSUME_BASE_TYPE(engine, DIRECT_TYPE);

	if (!(tmp_path = as<Ink_DirectPointer>(base)->path)) {
		InkWarn_IO_Uninitialized_Direct_Pointer(engine);
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, makeDir(tmp_path->c_str()));
}

Ink_Object *InkNative_Direct_Remove(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	string *tmp_path;

	ASSUME_BASE_TYPE(engine, DIRECT_TYPE);

	if (!(tmp_path = as<Ink_DirectPointer>(base)->path)) {
		InkWarn_IO_Uninitialized_Direct_Pointer(engine);
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, removeDir(tmp_path->c_str()) == 0);
}

Ink_Object *InkNative_Direct_Path(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	string *tmp_path;

	ASSUME_BASE_TYPE(engine, DIRECT_TYPE);

	if (!(tmp_path = as<Ink_DirectPointer>(base)->path)) {
		InkWarn_IO_Uninitialized_Direct_Pointer(engine);
		return NULL_OBJ;
	}

	return new Ink_String(engine, *tmp_path);
}

Ink_Object *InkNative_Direct_Each(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	string *tmp_path;
	Ink_Object **args = NULL;
	Ink_Object *ret_tmp;
	Ink_Array *ret = NULL;
	Ink_ArrayValue::size_type i;
	Ink_Object *block = NULL;
	IGC_CollectEngine *gc_engine = engine->getCurrentGC();

	ASSUME_BASE_TYPE(engine, DIRECT_TYPE);

	if (!(tmp_path = as<Ink_DirectPointer>(base)->path)) {
		InkWarn_IO_Uninitialized_Direct_Pointer(engine);
		return NULL_OBJ;
	}

	if (argc && argv[0]->type == INK_FUNCTION)
		block = argv[0];

	ret = new Ink_Array(engine);
	engine->addPardonObject(ret);

	args = (Ink_Object **)malloc(sizeof(Ink_Object *));

#if defined(INK_PLATFORM_LINUX)
	DIR *dir_handle = opendir(tmp_path->c_str());
	struct dirent *child;

	if (!dir_handle) {
		InkWarn_Direct_Not_Exist(engine, tmp_path->c_str());
		free(args);
		return NULL_OBJ;
	}

	while ((child = readdir(dir_handle)) != NULL) {

	#define CLOSE_HANDLER closedir(dir_handle)
	#define CHILD_NAME (child->d_name)
#elif defined(INK_PLATFORM_WIN32)
	WIN32_FIND_DATA data;
	HANDLE dir_handle = NULL;

	dir_handle = FindFirstFile((string(INK_MODULE_DIR) + "/*").c_str(), &data);

	if (dir_handle == INVALID_HANDLE_VALUE) {
		do {

	#define CLOSE_HANDLER FindClose(dir_handle)
	#define CHILD_NAME (data.cFileName)
#endif
	gc_engine->checkGC();
	if (block) {
		args[0] = new Ink_String(engine, string(CHILD_NAME));
		ret->value.push_back(new Ink_HashTable(ret_tmp = block->call(engine, context, 1, args)));
		if (engine->getSignal() != INTER_NONE) {
			switch (engine->getSignal()) {
				case INTER_RETURN:
					free(args);
					engine->removePardonObject(ret);
					CLOSE_HANDLER;
					return engine->getInterruptValue(); // signal penetrated
				case INTER_DROP:
				case INTER_BREAK:
					free(args);
					engine->removePardonObject(ret);
					CLOSE_HANDLER;
					return engine->trapSignal(); // trap the signal
				case INTER_CONTINUE:
					engine->trapSignal(); // trap the signal, but do not return
					continue;
				default:
					free(args);
					engine->removePardonObject(ret);
					CLOSE_HANDLER;
					return NULL_OBJ;
			}
		}
	} else {
		ret->value.push_back(new Ink_HashTable(new Ink_String(engine, string(CHILD_NAME))));
	}


#if defined(INK_PLATFORM_LINUX)
	}

	closedir(dir_handle);
#elif defined(INK_PLATFORM_WIN32)
		} while (FindNextFile(dir_handle, &data));
	} else {
		InkWarn_Direct_Not_Exist(engine, tmp_path->c_str());
		free(args);
		return NULL_OBJ;
	}

	FindClose(dir_handle);
#endif

	free(args);
	engine->removePardonObject(ret);

	return ret;
}

Ink_Object *InkNative_Direct_Exist_Static(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, isDirExist(as<Ink_String>(argv[0])->getValue().c_str()));
}

Ink_Object *InkNative_Direct_Create_Static(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, makeDir(as<Ink_String>(argv[0])->getValue().c_str()));
}

Ink_Object *InkNative_Direct_Remove_Static(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, removeDir(as<Ink_String>(argv[0])->getValue().c_str()) == 0);
}

void Ink_DirectPointer::Ink_DirectMethodInit(Ink_InterpreteEngine *engine)
{
	setSlot("exist", new Ink_FunctionObject(engine, InkNative_Direct_Exist));
	setSlot("create", new Ink_FunctionObject(engine, InkNative_Direct_Create));
	setSlot("remove", new Ink_FunctionObject(engine, InkNative_Direct_Remove));
	setSlot("path", new Ink_FunctionObject(engine, InkNative_Direct_Path));

#if defined(INK_PLATFORM_LINUX) || defined(INK_PLATFORM_WIN32)
	setSlot("each", new Ink_FunctionObject(engine, InkNative_Direct_Each));
#endif

	return;
}

void InkMod_Direct_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot("Directory", new Ink_FunctionObject(engine, InkNative_Direct_Constructor));
	bondee->setSlot("dir_exist", new Ink_FunctionObject(engine, InkNative_Direct_Exist_Static));
	bondee->setSlot("rmdir", new Ink_FunctionObject(engine, InkNative_Direct_Remove_Static));
	bondee->setSlot("mkdir", new Ink_FunctionObject(engine, InkNative_Direct_Create_Static));

	return;
}

Ink_Object *InkMod_Direct_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *apply_to = argv[1];
	
	InkMod_Direct_bondTo(engine, apply_to);

	return NULL_OBJ;
}