#ifndef _NATIVE_FILE_H
#define _NATIVE_FILE_H

#include <stdio.h>
#include "core/object.h"
#include "../../includes/universal.h"

#define FILE_GETS_BUFFER_SIZE 1000

extern Ink_TypeTag file_pointer_type_tag;

class Ink_FilePointer: public Ink_Object {
public:
	FILE *fp;

	Ink_FilePointer(FILE *fp = NULL)
	: fp(fp)
	{
		type = file_pointer_type_tag;
		setMethod();
	}

	void setMethod();
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