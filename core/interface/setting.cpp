#include <string.h>
#include <stdlib.h>
#include "setting.h"
#include "core/error.h"
#include "core/package/load.h"
#include "core/gc/collect.h"

namespace ink {

using namespace std;

Ink_InputSetting::Ink_InputSetting(const char *input_file_path, FILE *fp, bool close_fp)
: close_fp(close_fp), input_file_pointer(fp), code_mode(SOURCE_CODE), input_file_path(input_file_path), if_run(true)
{
	igc_collect_treshold = IGC_COLLECT_TRESHOLD_UNIT;
	dbg_print_detail = false;
}

inline bool isArg(const char *arg)
{
	if (strlen(arg) && arg[0] == '-') {
		return true;
	}
	return false;
}

#define IS_SINGLE_DASH_ARG(a) (arg == (a) && dash_count == 1)
#define IS_DOUBLE_DASH_ARG(a) (arg == (a) && dash_count == 2)
#define REPRINT_DASH (string(dash_count == 1 ? "-" : "--"))
#define REPRINT_ARG (REPRINT_DASH + arg)

inline void printUsage(const char *prog_path)
{
	fprintf(stderr, "Usage: %s [options] source_file [arguments]\n", prog_path);
	fprintf(stderr,
"Options:\n"
"  %-25s %s\n"
"  %-25s %s\n"
"  %-25s %s\n"
"  %-25s %s\n",
	"--help or -h",						"Display this usage page",
	"--mod-path=<path> or -m=<path>",	"Add module searching path",
	"--gc-treshold=<treshold>",			"Set collect treshold for garbage collector",
	"--debug or -d",					"Open debug mode(print more debug info when error occurs, optional value(true or false))");
}

/* return: if print usage */
inline bool processArg(Ink_InputSetting &setting, Ink_SizeType dash_count,
					   string arg, bool has_val = false, string val = "")
{
	if (dash_count > 2) {
		fprintf(stderr, "Too many dashes before option\n");
		setting.if_run = false;
		return true;
	}

	if (IS_SINGLE_DASH_ARG("h") || IS_DOUBLE_DASH_ARG("help")) {
		setting.if_run = false;
		return true;
	} else if (IS_SINGLE_DASH_ARG("m") || IS_DOUBLE_DASH_ARG("mod-path")) {
		if (has_val) {
			Ink_addModPath(val.c_str());
		} else {
			fprintf(stderr, "Option %s requires a value\n", REPRINT_ARG.c_str());
			setting.if_run = false;
			return true;
		}
	} else if (IS_DOUBLE_DASH_ARG("gc-treshold")) {
		if (has_val) {
			int tmp = atoi(val.c_str());
			if (tmp <= 0) {
				fprintf(stderr, "Failed to parsing value of option %s or it's not valid\n", REPRINT_ARG.c_str());
				setting.if_run = false;
				return true;
			} else {
				setting.igc_collect_treshold = tmp;
			}
		} else {
			fprintf(stderr, "Option %s requires a value\n", REPRINT_ARG.c_str());
			setting.if_run = false;
			return true;
		}
	} else if (IS_SINGLE_DASH_ARG("d") || IS_DOUBLE_DASH_ARG("debug")) {
		if (has_val) {
			if (val == "true") {
				setting.dbg_print_detail = true;
			} else if (val == "false") {
				setting.dbg_print_detail = false;
			} else {
				fprintf(stderr, "Unknown value given for option %s, requires boolean\n", REPRINT_ARG.c_str());
				setting.if_run = false;
				return true;
			}
		} else {
			setting.dbg_print_detail = true;
		}
	} else {
		fprintf(stderr, "Unknown option %s\n", REPRINT_ARG.c_str());
		setting.if_run = false;
		return true;
	}

	return false;
}

Ink_InputSetting Ink_InputSetting::parseArg(int argc, char **argv)
{
	int i;
	Ink_SizeType c_i;
	Ink_InputSetting ret = Ink_InputSetting();
	FILE *fp = NULL;
	vector<char *> tmp_argv = vector<char *>();

	for (i = 1; i < argc; i++) {
		if (!fp) {
			if (isArg(argv[i])) {
				// arguments dealing
				for (c_i = 0; c_i < strlen(argv[i]) && argv[i][c_i] == '-'; c_i++) ;

				string tmp = string(&argv[i][c_i], strlen(argv[i]) - c_i);
				string::size_type epos = tmp.find_first_of('=');
				bool tmp_ret;
				if (epos != string::npos) {
					tmp_ret = processArg(ret, c_i, tmp.substr(0, epos), true, tmp.substr(epos + 1));
				} else {
					tmp_ret = processArg(ret, c_i, tmp);
				}
				if (tmp_ret) {
					printUsage(argv[0]);
				}
			} else if (ret.if_run) { // file input
				if (!(fp = fopen(argv[i], "r"))) {
					InkError_Failed_Open_File(NULL, argv[i]);
					break;
				}
				ret.setFilePath(argv[i]);
				ret.setInput(fp);
				ret.close_fp = true;

				tmp_argv.push_back(argv[i]);
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

}
