#include <stdio.h>
#include <string>
#include "core/native/native.h"
#include "core/general.h"
#include "core/debug.h"
#include "file.h"

using namespace std;

Ink_TypeTag file_pointer_type_tag;

inline string getStringVal(Ink_Object *str)
{
	if (str->type == INK_STRING) {
		return as<Ink_String>(str)->value;
	}
	return "";
}

Ink_Object *InkNative_File_Exist(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(!access(getStringVal(argv[0]).c_str(), 0));
}

Ink_Object *InkNative_File_Remove(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(!remove(getStringVal(argv[0]).c_str()));
}

Ink_Object *InkNative_File_Constructor(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *ret;
	FILE *fp = NULL;
	const char *tmp;

	if (checkArgument(false, argc, argv, 2, INK_STRING, INK_STRING)) {
		fp = fopen(tmp = getStringVal(argv[0]).c_str(), getStringVal(argv[1]).c_str());
		if (!fp) {
			InkWarn_Failed_Open_File(tmp);
		}
	} else if (checkArgument(false, argc, argv, 1, INK_STRING)) {
		fp = fopen(tmp = getStringVal(argv[0]).c_str(), "r");
		if (!fp) {
			InkWarn_Failed_Open_File(tmp);
		}
	} else if (checkArgument(false, argc, argv, 1, file_pointer_type_tag)) {
		fp = as<Ink_FilePointer>(argv[0])->fp;
	}

	context->getLocal()->context->setSlot("this", ret = new Ink_FilePointer(fp));
	
	return ret;
}

void Ink_FilePointer::setMethod()
{
	setSlot("close", new Ink_FunctionObject(InkNative_File_Close));
	setSlot("puts", new Ink_FunctionObject(InkNative_File_PutString));
	setSlot("gets", new Ink_FunctionObject(InkNative_File_GetString));
	setSlot("flush", new Ink_FunctionObject(InkNative_File_Flush));
	return;
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
		fputs(getStringVal(argv[0]).c_str(), tmp);

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

Ink_Object *InkMod_File_Loader(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *global_context = context->getGlobal()->context;

	file_pointer_type_tag = (Ink_TypeTag)registerType("io.file.file_pointer");
	
	global_context->setSlot("File", new Ink_FunctionObject(InkNative_File_Constructor));
	global_context->setSlot("file_exist", new Ink_FunctionObject(InkNative_File_Exist));
	global_context->setSlot("file_remove", new Ink_FunctionObject(InkNative_File_Remove));

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