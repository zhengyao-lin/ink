#ifndef _INK_TIME_H_
#define _INK_TIME_H_

#include "../includes/universal.h"

#if defined(INK_PLATFORM_WIN32)
	#include <windows.h>
	
namespace ink {
	typedef int Ink_MilliSec;

	inline void Ink_msleep(Ink_MilliSec ms)
	{
		Sleep(ms);
		return;
	}

	inline Ink_MilliSec Ink_getCurrentMS()
	{
		return GetTickCount();
	}
}
#elif defined(INK_PLATFORM_LINUX)
	#include <unistd.h>
	#include <sys/time.h>

namespace ink {
	typedef int Ink_MilliSec;

	inline void Ink_msleep(Ink_MilliSec ms)
	{
		usleep(ms * 1000);
		return;
	}

	inline Ink_MilliSec Ink_getCurrentMS()
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}
}
#endif

#endif