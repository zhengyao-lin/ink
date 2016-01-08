#ifndef _SWITCHES_H_
#define _SWITCHES_H_

#include "universal.h"

#define INK_DEBUG_FLAG 1

#if defined(INK_PLATFORM_WIN32)
	#include <string.h>
	#include <windows.h>
	#define INK_PATH_SPLIT "/"
	#define INK_PATH_SPLIT_C '/'

	#define INK_TMP_PATH "./tmp"
#else
	#ifdef INK_INSTALL_PATH
		#define INK_MODULE_DIR INK_INSTALL_PATH "/lib/ink/modules"
	#else
		#define INK_MODULE_DIR "/usr/lib/ink/modules"
	#endif
	#define INK_TMP_PATH "/tmp/ink_tmp"
	#define INK_PATH_SPLIT "/"
	#define INK_PATH_SPLIT_C '/'
#endif


#if defined(INK_PLATFORM_LINUX)
	#include <unistd.h>
	#include <sys/stat.h>
	#include <dirent.h>

	inline bool isDirExist(const char *path)
	{
		DIR *dir;
		if ((dir = opendir(path)) != NULL) {
			closedir(dir);
			return true;
		}
		return false;
	}
#elif defined(INK_PLATFORM_WIN32)
	#include <io.h>

	inline bool isDirExist(const char *path)
	{
		return _access(path, 0) != -1;
	}
#endif

#endif