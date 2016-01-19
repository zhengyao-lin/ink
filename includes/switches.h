#ifndef _SWITCHES_H_
#define _SWITCHES_H_

#include "universal.h"

#define INK_DEBUG_FLAG 1
#define INK_ROOT_ACTOR_NAME "root"

#define INK_IS_BOTH_SPLIT(c) ((c) == '/' || (c) == '\\')

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
