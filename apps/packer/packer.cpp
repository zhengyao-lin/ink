#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include "core/error.h"
#include "core/package/load.h"

using namespace ink;
using namespace std;

inline void
InkWarn_Unknown_Argument(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, "Ink Packer: Unknown argument \"$(name)\"", name);
	return;
}

inline void
InkWarn_Excess_Argument(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, "Ink Packer: Excess argument \"$(name)\"", name);
	return;
}

inline void
InkError_No_Lib_Given(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintError(engine, -1, "Ink Packer: No library file given");
	return;
}

inline void
InkError_No_Dest_Given(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintError(engine, -1, "Ink Packer: No output file given");
	return;
}

inline void
InkError_Could_Not_Create_File(Ink_InterpreteEngine *engine, const char *path)
{
	stringstream strm;
	strm << "Ink Packer: Couldn't create file " << path;
	InkErro_doPrintError(engine, -1, strm.str().c_str());
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
			InkWarn_Unknown_Argument(NULL, instr_str.c_str());
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
			else InkWarn_Excess_Argument(NULL, argv[i]);
		} else {
			arg[argi++] = new Argument(argv[i]);
		}
	}

	for (i = 0; i < argi; i++) {
		if (arg[i]->instr == PACKAGE_NAME) {
			pack_name = arg[i]->arg;
		} else if (arg[i]->instr == AUTHOR) {
			author = arg[i]->arg;
		}
	}

	if (!so_file) {
		InkError_No_Lib_Given(NULL);
		// unreachable
	}

	if (!target) {
		InkError_No_Dest_Given(NULL);
		// unreachable
	}

	FILE *fp;
	fp = fopen(target, "wb");

	if (!fp) {
		InkError_Could_Not_Create_File(NULL, target);
		// unreachable
	}

	Ink_Package *pack = new Ink_Package(pack_name, author, so_file);
	pack->writeTo(fp);
	fflush(fp);
	fclose(fp);
	
	return 0;
}
