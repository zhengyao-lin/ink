#ifndef _SWITCHES_H_
#define _SWITCHES_H_

#define INK_DEBUG_FLAG 1

#ifdef __linux__
	#define INK_MODULE_DIR "/usr/lib/ink/modules"
	#define INK_TMP_PATH "/tmp/ink_tmp"
	#define INK_PATH_SPLIT "/"
	#define INK_PATH_SPLIT_C '/'
#else
	#define INK_PATH_SPLIT "\\"
	#define INK_PATH_SPLIT_C '\\'
#endif

#endif