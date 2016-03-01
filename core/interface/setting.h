#ifndef _SETTING_H_
#define _SETTING_H_

#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../general.h"
#include "../../includes/switches.h"

namespace ink {

enum Ink_CodeMode {
	SOURCE_CODE = 0,
	COMMANDER
};

class Ink_InputSetting {
public:
	bool close_fp;
	FILE *input_file_pointer;
	Ink_CodeMode code_mode;
	const char *input_file_path;
	std::vector<char *> argv;
	bool if_run;

	/* settings */
	IGC_ObjectCountType igc_collect_threshold;
	bool dbg_print_detail;

	Ink_InputSetting(const char *input_file_path = NULL, FILE *fp = stdin, bool close_fp = false);

	inline void setArgument(std::vector<char *> arg)
	{
		argv = arg;
		return;
	}

	inline std::vector<char *> getArgument()
	{
		return argv;
	}

	inline void setInput(FILE *fp)
	{
		input_file_pointer = fp;
		return;
	}

	inline FILE *getInput()
	{
		return input_file_pointer;
	}

	inline void setMode(Ink_CodeMode mode)
	{
		code_mode = mode;
		return;
	}

	inline Ink_CodeMode getMode()
	{
		return code_mode;
	}

	inline void setFilePath(const char *path)
	{
		input_file_path = path;
		return;
	}

	inline const char *getFilePath()
	{
		return  input_file_path;
	}

	inline void clean()
	{
		if (close_fp)
			fclose(input_file_pointer);
	}

	static Ink_InputSetting
	parseArg(int argc, char **argv);
};

}

#endif
