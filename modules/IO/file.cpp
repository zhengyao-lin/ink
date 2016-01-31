#include <stdio.h>
#include <string>
#include "core/general.h"
#include "core/debug.h"
#include "core/native/native.h"
#include "core/interface/engine.h"
#include "includes/switches.h"
#include "file.h"

using namespace ink;
using namespace std;

static InkMod_ModuleID ink_native_file_mod_id;

Ink_TypeTag getFilePointerType(Ink_InterpreteEngine *engine)
{
	return *engine->getEngineComAs<Ink_TypeTag>(ink_native_file_mod_id);
}

inline Ink_NumericValue getNumVal(Ink_Object *num)
{
	if (num->type == INK_NUMERIC) {
		return as<Ink_Numeric>(num)->value;
	}
	return 0;
}

Ink_Object *InkNative_File_Exist(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, !access(getStringVal(engine, context, argv[0])->getValue().c_str(), 0));
}

Ink_Object *InkNative_File_Remove(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, !remove(getStringVal(engine, context, argv[0])->getValue().c_str()));
}

Ink_Object *InkNative_File_Constructor(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *ret;
	FILE *fp = NULL;
	const char *tmp;

	if (checkArgument(false, argc, argv, 2, INK_STRING, INK_STRING)) {
		fp = fopen(tmp = getStringVal(engine, context, argv[0])->getValue().c_str(),
				   getStringVal(engine, context, argv[1])->getValue().c_str());
		if (!fp) {
			InkWarn_Failed_Open_File(engine, tmp);
		}
	} else if (checkArgument(false, argc, argv, 1, INK_STRING)) {
		fp = fopen(tmp = getStringVal(engine, context, argv[0])->getValue().c_str(), "r");
		if (!fp) {
			InkWarn_Failed_Open_File(engine, tmp);
		}
	} else if (checkArgument(false, argc, argv, 1, FILE_POINTER_TYPE)) {
		fp = as<Ink_FilePointer>(argv[0])->fp;
	}

	context->getLocal()->context->setSlot("this", ret = new Ink_FilePointer(engine, fp));
	
	return ret;
}

Ink_Object *InkNative_File_Close(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	FILE *tmp;

	ASSUME_BASE_TYPE(engine, FILE_POINTER_TYPE);

	tmp = as<Ink_FilePointer>(base)->fp;
	if (tmp) {
		as<Ink_FilePointer>(base)->fp = NULL;
		fclose(tmp);
	} else {
		InkWarn_IO_Uninitialized_File_Pointer(engine);
	}

	return NULL_OBJ;
}

Ink_Object *InkNative_File_PutString(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	FILE *tmp;

	ASSUME_BASE_TYPE(engine, FILE_POINTER_TYPE);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	tmp = as<Ink_FilePointer>(base)->fp;
	if (tmp) fputs(getStringVal(engine, context, argv[0])->getValue().c_str(), tmp);
	else {
		InkWarn_IO_Uninitialized_File_Pointer(engine);
	}

	return NULL_OBJ;
}

Ink_Object *InkNative_File_PutC(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	FILE *tmp;
	char ch;

	ASSUME_BASE_TYPE(engine, FILE_POINTER_TYPE);
	if (checkArgument(false, argc, argv, 1, INK_STRING)) {
		ch = getStringVal(engine, context, argv[0])->getValue().c_str()[0];
	} else if (checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		ch = getNumVal(argv[0]);
	} else {
		return NULL_OBJ;
	}

	tmp = as<Ink_FilePointer>(base)->fp;
	if (tmp) fputc(ch, tmp);
	else {
		InkWarn_IO_Uninitialized_File_Pointer(engine);
	}

	return NULL_OBJ;
}

Ink_Object *InkNative_File_GetString(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	FILE *tmp;
	char buffer[FILE_GETS_BUFFER_SIZE] = { '\0' };

	ASSUME_BASE_TYPE(engine, FILE_POINTER_TYPE);

	tmp = as<Ink_FilePointer>(base)->fp;
	if (tmp) {
		return new Ink_String(engine, string(fgets(buffer, FILE_GETS_BUFFER_SIZE, tmp)));
	}

	InkWarn_IO_Uninitialized_File_Pointer(engine);

	return NULL_OBJ;
}

Ink_Object *InkNative_File_GetC(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	FILE *tmp;

	ASSUME_BASE_TYPE(engine, FILE_POINTER_TYPE);

	tmp = as<Ink_FilePointer>(base)->fp;
	if (tmp) {
		return new Ink_Numeric(engine, fgetc(tmp));
	}

	InkWarn_IO_Uninitialized_File_Pointer(engine);

	return NULL_OBJ;
}

#if defined(INK_PLATFORM_LINUX)

Ink_Object *InkNative_File_GetCh(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	char ch;
	ttyMode mode_back = getttyMode(); // back up tty mode

	closettyBuffer();
	// close buffering
	ch = fgetc(stdin);

	// restore tty mode
	setttyMode(mode_back);

	return new Ink_Numeric(engine, ch);
}

#endif

Ink_Object *InkNative_File_ReadAll(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	FILE *tmp;
	size_t len;
	char *buffer;
	Ink_Object *ret;

	ASSUME_BASE_TYPE(engine, FILE_POINTER_TYPE);

	tmp = as<Ink_FilePointer>(base)->fp;

	if (tmp) {
		fseek(tmp, 0L, SEEK_END);
		len = ftell(tmp);
		buffer = (char *)malloc(len + 1);

		fseek(tmp, 0L, SEEK_SET);
		fread(buffer, len, 1, tmp);
		buffer[len] = '\0';

		ret = new Ink_String(engine, string(buffer));
		free(buffer);

		return ret;
	}

	InkWarn_IO_Uninitialized_File_Pointer(engine);

	return NULL_OBJ;
}

Ink_Object *InkNative_File_Flush(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	FILE *tmp;

	ASSUME_BASE_TYPE(engine, FILE_POINTER_TYPE);

	tmp = as<Ink_FilePointer>(base)->fp;
	if (tmp)
		fflush(tmp);
	else
		InkWarn_IO_Uninitialized_File_Pointer(engine);

	return NULL_OBJ;
}

void Ink_FilePointer::Ink_FilePointerMethodInit(Ink_InterpreteEngine *engine)
{
	setSlot("close", new Ink_FunctionObject(engine, InkNative_File_Close));
	setSlot("puts", new Ink_FunctionObject(engine, InkNative_File_PutString));
	setSlot("putc", new Ink_FunctionObject(engine, InkNative_File_PutC));
	setSlot("gets", new Ink_FunctionObject(engine, InkNative_File_GetString));
	setSlot("getc", new Ink_FunctionObject(engine, InkNative_File_GetC));
	setSlot("read", new Ink_FunctionObject(engine, InkNative_File_ReadAll));
	setSlot("flush", new Ink_FunctionObject(engine, InkNative_File_Flush));

	return;
}

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

void InkMod_Direct_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{

}

void InkMod_File_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot("File", new Ink_FunctionObject(engine, InkNative_File_Constructor));
	bondee->setSlot("file_exist", new Ink_FunctionObject(engine, InkNative_File_Exist));
	bondee->setSlot("file_remove", new Ink_FunctionObject(engine, InkNative_File_Remove));
#if defined(INK_PLATFORM_LINUX)
	bondee->setSlot("getch", new Ink_FunctionObject(engine, InkNative_File_GetCh)); // no buffering getc
#endif

	return;
}

void InkMod_IO_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot("stdin", new Ink_FilePointer(engine, stdin));
	bondee->setSlot("stdout", new Ink_FilePointer(engine, stdout));
	bondee->setSlot("stderr", new Ink_FilePointer(engine, stderr));

	return;
}

Ink_Object *InkMod_File_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *global_context = context->getGlobal()->context;
	
	InkMod_File_bondTo(engine, global_context);

	return NULL_OBJ;
}

Ink_Object *InkMod_Direct_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *global_context = context->getGlobal()->context;
	
	InkMod_Direct_bondTo(engine, global_context);

	return NULL_OBJ;
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
		DBG_CustomTypeType *type_p = NULL;

		if (!engine->getEngineComAs<Ink_TypeTag>(ink_native_file_mod_id)) {
			type_p = (DBG_CustomTypeType *)malloc(sizeof(DBG_CustomTypeType));
			*type_p = (DBG_CustomTypeType)engine->registerType("io.file.File");
			engine->addEngineCom(ink_native_file_mod_id, type_p);
			engine->addDestructor(Ink_EngineDestructor(InkNative_IO_EngineComCleaner, new com_cleaner_arg(ink_native_file_mod_id)));

			context->getGlobal()->context->setSlot("$io.file.File", tmp = new Ink_FilePointer(engine));
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
