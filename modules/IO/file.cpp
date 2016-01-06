#include <stdio.h>
#include <string>
#include "core/native/native.h"
#include "core/general.h"
#include "core/debug.h"
#include "includes/switches.h"
#include "file.h"

using namespace std;

Ink_TypeTag file_pointer_type_tag;

inline Ink_NumericValue getNumVal(Ink_Object *num)
{
	if (num->type == INK_NUMERIC) {
		return as<Ink_Numeric>(num)->value;
	}
	return 0;
}

Ink_Object *InkNative_File_Exist(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(!access(getStringVal(context, argv[0])->value.c_str(), 0));
}

Ink_Object *InkNative_File_Remove(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(!remove(getStringVal(context, argv[0])->value.c_str()));
}

Ink_Object *InkNative_File_Constructor(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *ret;
	FILE *fp = NULL;
	const char *tmp;

	if (checkArgument(false, argc, argv, 2, INK_STRING, INK_STRING)) {
		fp = fopen(tmp = getStringVal(context, argv[0])->value.c_str(), getStringVal(context, argv[1])->value.c_str());
		if (!fp) {
			InkWarn_Failed_Open_File(tmp);
		}
	} else if (checkArgument(false, argc, argv, 1, INK_STRING)) {
		fp = fopen(tmp = getStringVal(context, argv[0])->value.c_str(), "r");
		if (!fp) {
			InkWarn_Failed_Open_File(tmp);
		}
	} else if (checkArgument(false, argc, argv, 1, file_pointer_type_tag)) {
		fp = as<Ink_FilePointer>(argv[0])->fp;
	}

	context->getLocal()->context->setSlot("this", ret = new Ink_FilePointer(fp));
	
	return ret;
}

Ink_Object *InkNative_File_Close(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	FILE *tmp;

	ASSUME_BASE_TYPE(file_pointer_type_tag);

	tmp = as<Ink_FilePointer>(base)->fp;
	if (tmp)
		fclose(tmp);

	return NULL_OBJ;
}

Ink_Object *InkNative_File_PutString(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	FILE *tmp;

	ASSUME_BASE_TYPE(file_pointer_type_tag);
	if (!checkArgument(argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	tmp = as<Ink_FilePointer>(base)->fp;
	if (tmp)
		fputs(getStringVal(context, argv[0])->value.c_str(), tmp);

	return NULL_OBJ;
}

Ink_Object *InkNative_File_PutC(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	FILE *tmp;
	char ch;

	ASSUME_BASE_TYPE(file_pointer_type_tag);
	if (checkArgument(false, argc, argv, 1, INK_STRING)) {
		ch = getStringVal(context, argv[0])->value.c_str()[0];
	} else if (checkArgument(argc, argv, 1, INK_NUMERIC)) {
		ch = getNumVal(argv[0]);
	} else {
		return NULL_OBJ;
	}

	tmp = as<Ink_FilePointer>(base)->fp;
	if (tmp)
		fputc(ch, tmp);

	return NULL_OBJ;
}

Ink_Object *InkNative_File_GetString(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	FILE *tmp;
	char buffer[FILE_GETS_BUFFER_SIZE] = { '\0' };

	ASSUME_BASE_TYPE(file_pointer_type_tag);

	tmp = as<Ink_FilePointer>(base)->fp;
	if (tmp) {
		return new Ink_String(*StrPool_addStr(fgets(buffer, FILE_GETS_BUFFER_SIZE, tmp)));
	}

	return NULL_OBJ;
}

Ink_Object *InkNative_File_GetC(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	FILE *tmp;

	ASSUME_BASE_TYPE(file_pointer_type_tag);

	tmp = as<Ink_FilePointer>(base)->fp;
	if (tmp) {
		return new Ink_Numeric(fgetc(tmp));
	}

	return NULL_OBJ;
}

#ifdef __linux__

Ink_Object *InkNative_File_GetCh(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	char ch;
	ttyMode mode_back = getttyMode(); // back up tty mode

	closettyBuffer();
	// close buffering
	ch = fgetc(stdin);

	// restore tty mode
	setttyMode(mode_back);

	return new Ink_Numeric(ch);
}

#endif

Ink_Object *InkNative_File_ReadAll(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	FILE *tmp;
	size_t len;
	char *buffer;
	Ink_Object *ret;

	ASSUME_BASE_TYPE(file_pointer_type_tag);

	tmp = as<Ink_FilePointer>(base)->fp;

	if (tmp) {
		fseek(tmp, 0L, SEEK_END);
		len = ftell(tmp);
		buffer = (char *)malloc(len + 1);

		fseek(tmp, 0L, SEEK_SET);
		fread(buffer, len, 1, tmp);
		buffer[len] = '\0';

		ret = new Ink_String(*StrPool_addStr(buffer));
		free(buffer);

		return ret;
	}

	return NULL_OBJ;
}

Ink_Object *InkNative_File_Flush(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	FILE *tmp;

	ASSUME_BASE_TYPE(file_pointer_type_tag);

	tmp = as<Ink_FilePointer>(base)->fp;
	if (tmp)
		fflush(tmp);

	return NULL_OBJ;
}

void Ink_FilePointer::setMethod()
{
	setSlot("close", new Ink_FunctionObject(InkNative_File_Close));
	setSlot("puts", new Ink_FunctionObject(InkNative_File_PutString));
	setSlot("putc", new Ink_FunctionObject(InkNative_File_PutC));
	setSlot("gets", new Ink_FunctionObject(InkNative_File_GetString));
	setSlot("getc", new Ink_FunctionObject(InkNative_File_GetC));
	setSlot("read", new Ink_FunctionObject(InkNative_File_ReadAll));
	setSlot("flush", new Ink_FunctionObject(InkNative_File_Flush));
	return;
}

Ink_Object *InkMod_File_Loader(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *global_context = context->getGlobal()->context;

	file_pointer_type_tag = (Ink_TypeTag)DBG_registerType("io.file.file_pointer");
	
	global_context->setSlot("File", new Ink_FunctionObject(InkNative_File_Constructor));
	global_context->setSlot("file_exist", new Ink_FunctionObject(InkNative_File_Exist));
	global_context->setSlot("file_remove", new Ink_FunctionObject(InkNative_File_Remove));
#ifdef __linux__
	global_context->setSlot("getch", new Ink_FunctionObject(InkNative_File_GetCh)); // no buffering getc
#endif

	return NULL_OBJ;
}

Ink_Object *InkMod_IO_Loader(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *global_context = context->getGlobal()->context;

	if (argc)
		argv[0]->getSlot("file")->getSlot("load")->call(context);
	global_context->setSlot("stdin", new Ink_FilePointer(stdin));
	global_context->setSlot("stdout", new Ink_FilePointer(stdout));
	global_context->setSlot("stderr", new Ink_FilePointer(stderr));

	return NULL_OBJ;
}

extern "C" {
	void InkMod_Loader(Ink_ContextChain *context)
	{
		addPackage(addPackage(context, "io", new Ink_FunctionObject(InkMod_IO_Loader)),
				   "file", new Ink_FunctionObject(InkMod_File_Loader));
	}
}