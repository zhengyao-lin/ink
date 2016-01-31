#ifndef _NATIVE_FILE_H
#define _NATIVE_FILE_H

#include <stdio.h>
#include "core/object.h"
#include "../../includes/universal.h"

#define FILE_GETS_BUFFER_SIZE 1000
#define FILE_POINTER_TYPE (getFilePointerType(engine))

#if defined(INK_PLATFORM_LINUX)
	#include <sys/types.h>
	#include <dirent.h>
#elif defined(INK_PLATFORM_WIN32)
	#include <windows.h>
#endif

using namespace ink;

Ink_TypeTag getFilePointerType(Ink_InterpreteEngine *engine);

class Ink_FilePointer: public Ink_Object {
public:
	FILE *fp;

	Ink_FilePointer(Ink_InterpreteEngine *engine, FILE *fp = NULL)
	: Ink_Object(engine), fp(fp)
	{
		type = FILE_POINTER_TYPE;
		// setMethod(engine);
	}

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_FilePointerMethodInit(engine);
	}
	void Ink_FilePointerMethodInit(Ink_InterpreteEngine *engine);

	virtual ~Ink_FilePointer()
	{
		if (fp && fp != stdin && fp != stdout && fp != stderr) fclose(fp);
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

inline void
InkWarn_IO_Uninitialized_File_Pointer(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "Operating uninitialized file pointer");
	return;
}

#endif
