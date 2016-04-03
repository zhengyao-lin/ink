#ifndef _MOD_FILE_H
#define _MOD_FILE_H

#include <stdio.h>
#include <string>
#include "core/object.h"
#include "core/error.h"
#include "core/package/load.h"
#include "../../includes/universal.h"

#define FILE_GETS_BUFFER_SIZE 1000
#define FILE_POINTER_TYPE (getFilePointerType(engine))
#define DIRECT_TYPE (getDirectType(engine))
#define NOT_STDIO(fp) ((fp) != stdout && (fp) != stdin && (fp) != stderr)

#if defined(INK_PLATFORM_LINUX)
	#include <sys/types.h>
	#include <dirent.h>
#elif defined(INK_PLATFORM_WIN32)
	#include <windows.h>
#endif

using namespace ink;

struct com_cleaner_arg {
	Ink_ModuleID id;
	com_cleaner_arg(Ink_ModuleID id)
	: id(id)
	{ }
};

struct com_struct {
	Ink_TypeTag file_type;
	Ink_TypeTag direct_type;

	com_struct()
	: file_type(-1), direct_type(-1)
	{ }
};

void InkMod_IO_EngineComCleaner(Ink_InterpreteEngine *engine, void *arg);

Ink_TypeTag getFilePointerType(Ink_InterpreteEngine *engine);
Ink_TypeTag getDirectType(Ink_InterpreteEngine *engine);

void InkMod_File_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee);
void InkMod_IO_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee);
void InkMod_Direct_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee);

Ink_Object *InkMod_IO_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);
Ink_Object *InkMod_File_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);
Ink_Object *InkMod_Direct_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);

class Ink_FilePointer: public Ink_Object {
public:
	FILE *fp;

	Ink_FilePointer(Ink_InterpreteEngine *engine, FILE *fp = NULL)
	: Ink_Object(engine), fp(fp)
	{
		type = FILE_POINTER_TYPE;
		initProto(engine);
		// setMethod(engine);
	}

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_FilePointerMethodInit(engine);
	}
	void Ink_FilePointerMethodInit(Ink_InterpreteEngine *engine);

	virtual ~Ink_FilePointer()
	{
		if (fp && NOT_STDIO(fp)) fclose(fp);
	}
};

class Ink_DirectPointer: public Ink_Object {
public:
	std::string *path;

	Ink_DirectPointer(Ink_InterpreteEngine *engine, std::string path)
	: Ink_Object(engine), path(new std::string(path))
	{
		type = DIRECT_TYPE;
		initProto(engine);
	}

	Ink_DirectPointer(Ink_InterpreteEngine *engine)
	: Ink_Object(engine), path(NULL)
	{
		type = DIRECT_TYPE;
		initProto(engine);
	}

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_DirectMethodInit(engine);
	}
	void Ink_DirectMethodInit(Ink_InterpreteEngine *engine);

	virtual ~Ink_DirectPointer()
	{
		if (path) delete path;
	}
};

#if defined(INK_PLATFORM_LINUX)
	#include <termios.h>

	typedef struct termios ttyMode;

	inline ttyMode getttyMode()
	{
	    ttyMode mode;
	    tcgetattr(0, &mode);
	    return mode;
	}

	inline void setttyMode(ttyMode mode)
	{
		tcsetattr(0, TCSANOW, &mode);
		return;
	}

	inline void closettyBuffer()
	{
		ttyMode state;

		tcgetattr(0, &state);

		state.c_lflag &= ~ICANON;
		state.c_cc[VMIN] = 1;

		tcsetattr(0, TCSANOW, &state);

		return;
	}
#endif

extern Ink_ModuleID ink_native_io_mod_id;

enum InkMod_IO_ExceptionCode {
	INK_EXCODE_WARN_IO_UNINITIALIZED_FILE_POINTER = INK_EXCODE_CUSTOM_START,
	INK_EXCODE_WARN_IO_UNINITIALIZED_DIRECT_POINTER,
	INK_EXCODE_WARN_DIRECT_NOT_EXIST,
	INK_EXCODE_WARN_FILE_FAILED_READ_FILE
};

inline void
InkWarn_IO_Uninitialized_File_Pointer(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_io_mod_id,
						   INK_EXCODE_WARN_IO_UNINITIALIZED_FILE_POINTER,
						   "Operating uninitialized file pointer(has closed or destructed)");
	return;
}

inline void
InkWarn_IO_Uninitialized_Direct_Pointer(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_io_mod_id,
						   INK_EXCODE_WARN_IO_UNINITIALIZED_DIRECT_POINTER,
						   "Operating uninitialized directory pointer(is it a prototype?)");
	return;
}

inline void
InkWarn_Direct_Not_Exist(Ink_InterpreteEngine *engine, const char *path)
{
	InkErro_doPrintWarning(engine, ink_native_io_mod_id,
						   INK_EXCODE_WARN_DIRECT_NOT_EXIST,
						   "Directory $(path) does not exist", path);
	return;
}

inline void
InkWarn_File_Failed_Read_File(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_io_mod_id,
						   INK_EXCODE_WARN_FILE_FAILED_READ_FILE,
						   "Failed to read file");
	return;
}

#endif
