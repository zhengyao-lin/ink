#ifndef _SETTING_H_
#define _SETTING_H_

#include <stdio.h>

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