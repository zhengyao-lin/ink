#ifndef _SETTING_H_
#define _SETTING_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __linux__
	#define PATH_SPLIT '/'
	#include <unistd.h>

	inline char *getCurrentDir()
	{
		return getcwd(NULL, 0);
	}

	inline int changeDir(const char *path)
	{
		return chdir(path);
	}
#else
	#define PATH_SPLIT '\\'
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
		if (path[i] == PATH_SPLIT)
			break;
	}

	if (i >= 0) {
		ret = (char *)malloc(sizeof(char) * (i + 2));
		memset(ret, 0x0, sizeof(char) * (i + 2));
		strncpy(ret, path, i + 1);
	}

	return ret;
}

class Ink_InputSetting {
public:
	bool close_fp;
	FILE *input_file_pointer;

	Ink_InputSetting(FILE *fp = stdin, bool close_fp = false)
	: close_fp(close_fp), input_file_pointer(fp)
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

	void clean()
	{
		if (close_fp)
			fclose(input_file_pointer);
	}

	static Ink_InputSetting
	parseArg(int argc, char **argv);
};

#endif