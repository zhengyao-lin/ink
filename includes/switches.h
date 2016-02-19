#ifndef _SWITCHES_H_
#define _SWITCHES_H_

#include <string.h>
#include "universal.h"

#define INK_ROOT_ACTOR_NAME "root"
#define INK_ARGV_NAME "ARGV"

#if defined(INK_PLATFORM_WIN32)
	#define INK_PATH_SPLIT "\\"
	#define INK_PATH_SPLIT_C '\\'
	#define INK_LINE_SEP "\r\n"
#else
	#define INK_PATH_SPLIT "/"
	#define INK_PATH_SPLIT_C '/'
	#define INK_LINE_SEP "\n"
#endif

#define INK_IS_BOTH_SPLIT(c) ((c) == '/' || (c) == '\\')

namespace ink {
inline char *getBasePath(const char *path)
{
	char *ret = NULL;
	int i;

	for (i = strlen(path) - 1; i > 0; i--) {
		if (INK_IS_BOTH_SPLIT(path[i]))
			break;
	}

	if (i >= 0) {
		ret = (char *)malloc(sizeof(char) * (i + 2));
		memset(ret, 0x0, sizeof(char) * (i + 2));
		strncpy(ret, path, i + 1);
	}

	return ret;
}
}

#if defined(INK_PLATFORM_LINUX)
	#include <unistd.h>
	#include <sys/stat.h>
	#include <dirent.h>

namespace ink {
	inline bool isDirExist(const char *path)
	{
		DIR *dir;
		if ((dir = opendir(path)) != NULL) {
			closedir(dir);
			return true;
		}
		return false;
	}
	inline bool makeDir(const char *path)
	{
		return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
	}
}
#elif defined(INK_PLATFORM_WIN32)
	#include <windows.h>
	#include <io.h>
	#include <direct.h>

namespace ink {
	inline bool isDirExist(const char *path)
	{
		return _access(path, 0) != -1;
	}
	inline bool makeDir(const char *path)
	{
		return _mkdir(path) == 0;
	}
}
#endif

#endif
