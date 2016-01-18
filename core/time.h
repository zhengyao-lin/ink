#ifndef _INK_TIME_H_
#define _INK_TIME_H_

#include "../includes/universal.h"

#if defined(INK_PLATFORM_WIN32)
	#include <windows.h>
	
	typedef DWORD Ink_MicroSec;

	inline void Ink_usleep(Ink_MicroSec ms)
	{
		Sleep(ms);
		return;
	}
#elif defined(INK_PLATFORM_LINUX)
	#include <unistd.h>
	
	typedef int Ink_MicroSec;

	inline void Ink_usleep(Ink_MicroSec ms)
	{
		usleep(ms);
		return;
	}
#endif

#endif