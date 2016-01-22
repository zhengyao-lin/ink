#include <string.h>
#include "setting.h"
#include "core/error.h"

inline bool isArg(const char *arg)
{
	if (strlen(arg) && arg[0] == '-') {
		return true;
	}
	return false;
}

Ink_InputSetting Ink_InputSetting::parseArg(int argc, char **argv)
{
	int i;
	Ink_InputSetting ret = Ink_InputSetting();
	FILE *fp = NULL;
	vector<char *> tmp_argv = vector<char *>();

	for (i = 1; i < argc; i++) {
		if (!fp) {
			if (isArg(argv[i])) {
				// arguments dealing
			} else { // file input
				if (!(fp = fopen(argv[i], "r"))) {
					InkErr_Failed_Open_File(NULL, argv[i]);
					break;
				}
				ret.setFilePath(argv[i]);
				ret.setInput(fp);
				ret.close_fp = true;
			}
		} else {
			tmp_argv.push_back(argv[i]);
		}
	}

	if (!fp) {
		ret.setMode(COMMANDER);
		ret.setInput(stdin);
	}
	ret.setArgument(tmp_argv);

	return ret;
}
