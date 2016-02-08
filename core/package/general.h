#ifndef _PACK_GENERAL_H_
#define _PACK_GENERAL_H_

#include <string.h>
#include "../../includes/switches.h"
#include "../../includes/universal.h"

#if defined(INK_PLATFORM_LINUX)

	#define INK_DL_SUFFIX "so"
	#define INK_TMP_PATH "/tmp/ink_tmp"

#elif defined(INK_PLATFORM_WIN32)

	#define INK_DL_SUFFIX "dll"
	#define INK_TMP_PATH ((getProgPath() + "tmp").c_str())

#endif

namespace ink {

inline bool createDirIfNotExist(const char *path) /* return: if exist */
{
	if (!isDirExist(path)) {
		makeDir(path);
		return false;
	}
	return true;
}

inline bool
hasSuffix(const char *path, const char *suf)
{
	long i, j;
	for (i = strlen(path), j = strlen(suf);
		 i >= 0 && j >= 0; i--, j--) {
		if (path[i] != suf[j])
			return false;
	}
	if (i > 0 && path[i] == '.' && j < 0) return true;
	return false;
}

}

#endif
