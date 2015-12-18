#ifndef _SETTING_H_
#define _SETTING_H_

#include <stdio.h>

class Ink_InputSetting {
public:
	FILE *input_file_pointer;

	Ink_InputSetting(FILE *fp = stdin)
	: input_file_pointer(fp)
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

	static Ink_InputSetting
	parseArg(int argc, char **argv);
};

#endif