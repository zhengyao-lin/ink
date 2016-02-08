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
InkWarn_Unknown_Version(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, "Ink Packer: Unknown version name \"$(name)\"", name);
	return;
}

inline void
InkError_Unknown_Version(Ink_InterpreteEngine *engine, const char *path)
{
	InkErro_doPrintError(engine, -1, "Ink Packer: Cannot detect module version of file $(path), please specify one", path);
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
	AUTHOR,
	VERSION
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
		} else if (!strcmp(instr_str.c_str(), "version")) {
			instr = VERSION;
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

Ink_MagicNumber getVersionBySuffix(char *path)
{
	auto i = strlen(path);

	if (i >= 3
		&& path[i - 1] == 'o'
		&& path[i - 2] == 's'
		&& path[i - 3] == '.') {
		return INK_MAGIC_NUM_0_LINUX;
	}

	if (i >= 4
		&& path[i - 1] == 'l'
		&& path[i - 2] == 'l'
		&& path[i - 3] == 'd'
		&& path[i = 4] == '.') {
		return INK_MAGIC_NUM_0_WINDOWS;
	}

	return INK_MAGIC_NUM_INVALID;
}

Ink_MagicNumber getVersionByName(char *name)
{
	if (!strcmp(name, "0_linux"))
		return INK_MAGIC_NUM_0_LINUX;
	else if (!strcmp(name, "0_win32"))
		return INK_MAGIC_NUM_0_WINDOWS;

	return INK_MAGIC_NUM_INVALID;
}

int main(int argc, char **argv)
{
	int i, argi = 0;
	const char *pack_name = "Unknown", *author = "Anonymous";
	char *dest = NULL;
	Argument *arg[MAX_ARG_COUNT];
	Ink_MagicNumber tmp_num;

	vector<Ink_MagicNumber> file_version = vector<Ink_MagicNumber>();
	vector<char *> files = vector<char *>();
	vector<char *>::size_type fi;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			files.push_back(argv[i]);
			file_version.push_back(INK_MAGIC_NUM_INVALID);
		} else {
			arg[argi++] = new Argument(argv[i]);
			if (arg[argi - 1]->instr == VERSION) {
				tmp_num = getVersionByName(arg[argi - 1]->arg);
				if (tmp_num == INK_MAGIC_NUM_INVALID) {
					InkWarn_Unknown_Version(NULL, arg[argi - 1]->arg);
				} else {
					file_version[file_version.size() - 1] = tmp_num;
				}
			}
		}
	}

	for (i = 0; i < argi; i++) {
		if (arg[i]->instr == PACKAGE_NAME) {
			pack_name = arg[i]->arg;
		} else if (arg[i]->instr == AUTHOR) {
			author = arg[i]->arg;
		}
	}

	if (!files.size()) {
		InkError_No_Lib_Given(NULL);
		// unreachable
	}

	if (files.size() <= 1) {
		InkError_No_Dest_Given(NULL);
		// unreachable
	}

	dest = files[files.size() - 1];

	FILE *fp;
	fp = fopen(dest, "wb");

	if (!fp) {
		InkError_Could_Not_Create_File(NULL, dest);
		// unreachable
	}

	Ink_Package *pack = new Ink_Package(pack_name, author);

	for (fi = 0; fi < files.size() - 1; fi++) {
		tmp_num = file_version[fi] == INK_MAGIC_NUM_INVALID
				  ? getVersionBySuffix(files[fi])
				  : file_version[fi];
		if (tmp_num == INK_MAGIC_NUM_INVALID) {
			InkError_Unknown_Version(NULL, files[fi]);
			// unreachable
		}
		pack->addDLFile(files[fi], tmp_num);
	}

	pack->writeTo(fp);
	fflush(fp);
	fclose(fp);
	
	return 0;
}
