#ifndef _NATIVE_FILE_H
#define _NATIVE_FILE_H

#include <stdio.h>
#include "core/object.h"
#include "../../includes/universal.h"

#define FILE_GETS_BUFFER_SIZE 1000
#define FILE_POINTER_TYPE (getFilePointerType(engine))

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

#endif
