#include <string.h>
#include <stdlib.h>
#include <string>
#include "core/error.h"
#include "core/package/load.h"

using namespace std;

inline void
InkWarn_Unknown_Argument(const char *name)
{
	InkWarn_doPrintWarning("Ink Packer: Unknown argument \"$(name)\"", name);
	return;
}

inline void
InkWarn_Excess_Argument(const char *name)
{
	InkWarn_doPrintWarning("Ink Packer: Excess argument \"$(name)\"", name);
	return;
}

inline void
InkErr_No_Lib_Given()
{
	InkErr_doPrintError(NULL, "Ink Packer: No library file given");
	return;
}

inline void
InkErr_No_Dest_Given()
{
	InkErr_doPrintError(NULL, "Ink Packer: No output file given");
	return;
}

inline void
InkErr_Could_Not_Create_File(const char *path)
{
	InkErr_doPrintError(NULL, "Ink Packer: Couldn't create file $(path)", path);
	return;
}

enum Instr {
	UNKNOWN = 0,
	PACKAGE_NAME,
	AUTHOR
};

class Argument {
public:
	Instr instr;
	char *arg;

	Argument(char *origin)
	{
		int i, len = strlen(origin), start;
		string instr_str;

		for (i = 0; i < len && origin[i] == '-'; i++) ;
		start = i;

		for (; i < len; i++) {
			if (origin[i] == '=') {
				break;
			}
		}

		arg = NULL;
		if (i < len) {
			instr_str = string(origin).substr(start, i - 2);
		} else {
			instr_str = string(origin).substr(start);
			return;
		}

		if (!strcmp(instr_str.c_str(), "package-name")) {
			instr = PACKAGE_NAME;
		} else if (!strcmp(instr_str.c_str(), "author")) {
			instr = AUTHOR;
		} else {
			instr = UNKNOWN;
			InkWarn_Unknown_Argument(instr_str.c_str());
			return;
		}

		arg = (char *)malloc(sizeof(char) * (len - i));
		strcpy(arg, string(origin).substr(i + 1, len - i - 1).c_str());
	}

	~Argument()
	{
		if (arg)
			free(arg);
	}
};

#define MAX_ARG_COUNT 1000

int main(int argc, char **argv)
{
	int i, argi = 0;
	const char *so_file = NULL, *pack_name = "Unknown", *author = "Anonymous";
	char *target = NULL;
	Argument *arg[MAX_ARG_COUNT];

	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			if (!so_file)
				so_file = argv[i];
			else if (!target)
				target = argv[i];
			else InkWarn_Excess_Argument(argv[i]);
		} else {
			arg[argi++] = new Argument(argv[i]);
		}
	}

	for (i = 0; i < argi; i++) {
		if (arg[i]->instr == PACKAGE_NAME) {
			pack_name = arg[i]->arg;
		} else if (arg[i]->instr == PACKAGE_NAME) {
			author = arg[i]->arg;
		}
	}

	if (!so_file) {
		InkErr_No_Lib_Given();
		// unreachable
	}

	if (!target) {
		InkErr_No_Dest_Given();
		// unreachable
	}

	FILE *fp;
	fp = fopen(target, "wbx");

	if (!fp) {
		InkErr_Could_Not_Create_File(target);
		// unreachable
	}

	Ink_Package *pack = new Ink_Package(pack_name, author, so_file);
	pack->writeTo(fp);
	fflush(fp);
	fclose(fp);
	
	return 0;
}