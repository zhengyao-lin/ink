#ifndef _SETTING_H_
#define _SETTING_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../includes/switches.h"

#if defined(INK_PLATFORM_LINUX)
	#include <unistd.h>

	inline char *getCurrentDir()
	{
		return getcwd(NULL, 0);
	}

	inline int changeDir(const char *path)
	{
		return chdir(path);
	}
#elif defined(INK_PLATFORM_WIN32)
	#include <direct.h>

	inline char *getCurrentDir()
	{
		return _getcwd(NULL, 0);
	}

	inline int changeDir(const char *path)
	{
		return _chdir(path);
	}
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

enum Ink_CodeMode {
	SOURCE_CODE = 0,
	COMMANDER
};

class Ink_InputSetting {
public:
	bool close_fp;
	FILE *input_file_pointer;
	Ink_CodeMode code_mode;

	Ink_InputSetting(FILE *fp = stdin, bool close_fp = false)
	: close_fp(close_fp), input_file_pointer(fp), code_mode(SOURCE_CODE)
	{ }

	void setInput(FILE *fp)
	{
		input_file_pointer = fp;
		return;
	}

	FILE *getInput()
	{
		return input_file_pointer;
	}

	void setMode(Ink_CodeMode mode)
	{
		code_mode = mode;
		return;
	}

	Ink_CodeMode getMode()
	{
		return code_mode;
	}

	void clean()
	{
		if (close_fp)
			fclose(input_file_pointer);
	}

	static Ink_InputSetting
	parseArg(int argc, char **argv);
};

#endif