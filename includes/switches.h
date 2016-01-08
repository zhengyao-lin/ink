#ifndef _SWITCHES_H_
#define _SWITCHES_H_

#include "universal.h"

#define INK_DEBUG_FLAG 1

extern char *tmp_prog_path;

#if defined(INK_PLATFORM_WIN32)
	#include <string>
	#include <string.h>
	#include <windows.h>
	#define INK_MODULE_DIR ((getProgPath() + "modules").c_str())
	#define INK_PATH_SPLIT "\\"
	#define INK_PATH_SPLIT_C '\\'

	#define INK_TMP_PATH ((getProgPath() + "tmp").c_str())

	inline char *getBasePath(const char *path);
	inline std::string getProgPath()
	{
		if (tmp_prog_path) return std::string(tmp_prog_path);
		char buffer[MAX_PATH + 1];
		GetModuleFileName(NULL, buffer, MAX_PATH);
		return std::string(tmp_prog_path = getBasePath(buffer));
	}
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

inline char *getBasePath(const char *path)
{
	char *ret = NULL;
	int i;

	for (i = strlen(path) - 1; i > 0; i--) {
		if (path[i] == INK_PATH_SPLIT_C)
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