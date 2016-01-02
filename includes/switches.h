#ifndef _SWITCHES_H_
#define _SWITCHES_H_

#define INK_DEBUG_FLAG 1

#ifdef __linux__
	#ifdef INK_INSTALL_PATH
		#define INK_MODULE_DIR INK_INSTALL_PATH "/lib/ink/modules"
	#else
		#define INK_MODULE_DIR "/usr/lib/ink/modules"
	#endif
	#define INK_TMP_PATH "/tmp/ink_tmp"
	#define INK_PATH_SPLIT "/"
	#define INK_PATH_SPLIT_C '/'
#else
	#define INK_PATH_SPLIT "\\"
	#define INK_PATH_SPLIT_C '\\'
#endif


#ifdef __linux__
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
#endif

#endif