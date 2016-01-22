#ifndef _PKG_LOAD_H_
#define _PKG_LOAD_H_

#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../context.h"
#include "../error.h"
#include "../../includes/switches.h"
#include "../../includes/universal.h"

#if defined(INK_PLATFORM_WIN32)
	#include <string>
	#include <string.h>
	#include <windows.h>
	#define INK_MODULE_DIR ((getProgPath() + "modules").c_str())
	#define INK_PATH_SPLIT "\\"
	#define INK_PATH_SPLIT_C '\\'

	#define INK_TMP_PATH ((getProgPath() + "tmp").c_str())

	std::string getProgPath();
#else
	#ifdef INK_INSTALL_PATH
		#define INK_MODULE_DIR INK_INSTALL_PATH "/lib/ink/modules"
	#else
		#define INK_MODULE_DIR "/usr/lib/ink/modules"
	#endif
	#define INK_TMP_PATH "/tmp/ink_tmp"
	#define INK_PATH_SPLIT "/"
	#define INK_PATH_SPLIT_C '/'
#endif

#if defined(INK_PLATFORM_LINUX)
	#include <sys/types.h>
	#include <dirent.h>
	#include <dlfcn.h>

	#define INK_DL_SUFFIX "so"
	#define INK_DL_HANDLER void *
	#define INK_DL_OPEN(path, p) (dlopen((path), (p)))
	#define INK_DL_SYMBOL(handler, name) (dlsym((handler), (name)))
	#define INK_DL_CLOSE(handler) (dlclose(handler))
	#define INK_DL_ERROR() (dlerror())

	#define INK_DEFAULT_MAGIC_NUM INK_0_LINUX
#elif defined(INK_PLATFORM_WIN32)
	#include "winbase.h"

	#define INK_DL_SUFFIX "dll"
	#define INK_DL_HANDLER HINSTANCE
	#define INK_DL_OPEN(path, p) (LoadLibrary(path))
	#define INK_DL_SYMBOL(handler, name) (GetProcAddress((handler), (name)))
	#define INK_DL_CLOSE(handler) (FreeLibrary(handler))
	#define INK_DL_ERROR() (NULL)

	#define INK_DEFAULT_MAGIC_NUM INK_0_WINDOWS
#endif

using namespace std;

inline bool
hasSuffix(const char *path, const char *suf)
{
	int i, j;
	for (i = strlen(path), j = strlen(suf);
		 i >= 0 && j >= 0; i--, j--) {
		if (path[i] != suf[j])
			return false;
	}
	if (i > 0 && path[i] == '.' && j < 0) return true;
	return false;
}

typedef long InkMod_ModuleID;
typedef void (*InkMod_Loader)(Ink_InterpreteEngine *engine, Ink_ContextChain *context);
typedef int (*InkMod_Init)(InkMod_ModuleID id);
typedef vector<INK_DL_HANDLER> DLHandlerPool;
typedef size_t InkPack_Size;
typedef unsigned char byte;

enum Ink_MagicNumber {
	INK_0_LINUX = 0,
	INK_0_WINDOWS
};

class InkPack_String {
public:
	InkPack_Size len;
	char *str;

	InkPack_String(char *s, bool if_copy = true)
	{
		len = strlen(s);
		if (if_copy) {
			str = (char *)malloc(sizeof(char) * (len + 1));
			strcpy(str, s);
		} else {
			str = s;
		}
	}

	InkPack_String(const char *s)
	{
		len = strlen(s);
		str = (char *)malloc(sizeof(char) * (len + 1));
		strcpy(str, s);
	}

	void writeTo(FILE *fp)
	{
		fwrite(&len, sizeof(InkPack_Size), 1, fp);
		fwrite(str, len + 1, 1, fp);
	}

	static InkPack_String *readFrom(FILE *fp);

	~InkPack_String()
	{
		free(str);
	}
};

class InkPack_Info {
public:
	InkPack_String *pack_name;
	InkPack_String *author;

	InkPack_Info(const char *pack_name, const char *author)
	: pack_name(new InkPack_String(pack_name)), author(new InkPack_String(author))
	{ }

	InkPack_Info(InkPack_String *pack_name, InkPack_String *author)
	: pack_name(pack_name), author(author)
	{ }

	void writeTo(FILE *fp)
	{
		pack_name->writeTo(fp);
		author->writeTo(fp);
	}

	static InkPack_Info *readFrom(FILE *fp);

	~InkPack_Info()
	{
		delete pack_name;
		delete author;
	}
};

class InkPack_FileBlock {
public:
	InkPack_Size file_size;
	byte *data;

	InkPack_FileBlock(FILE *fp)
	{
		fseek(fp, 0L, SEEK_END);
		file_size = ftell(fp);

		data = (byte *)malloc(sizeof(byte) * file_size);
		fseek(fp, 0L, SEEK_SET);
		fread(data, sizeof(byte), file_size, fp);
	}

	InkPack_FileBlock(InkPack_Size size, byte *d)
	: file_size(size), data(d)
	{ }

	void writeTo(FILE *fp)
	{
		fwrite(&file_size, sizeof(InkPack_Size), 1, fp);
		fwrite(data, sizeof(byte) * file_size, 1, fp);
	}

	string *bufferToTmp(const char *file_suffix = "." INK_DL_SUFFIX); // return: tmp file path
	static InkPack_FileBlock *readFrom(FILE *fp);

	~InkPack_FileBlock()
	{
		free(data);
	}
};

class Ink_Package {
public:
	Ink_MagicNumber magic_num;
	InkPack_Info *pack_info;
	InkPack_FileBlock *dl_file;

	Ink_Package(const char *pack_name, const char *author, const char *dl_file_path, Ink_MagicNumber magic_num = INK_DEFAULT_MAGIC_NUM)
	: magic_num(magic_num)
	{
		FILE *fp;

		pack_info = new InkPack_Info(pack_name, author);
		fp = fopen(dl_file_path, "rb");
		if (fp) {
			dl_file = new InkPack_FileBlock(fp);
		} else {
			InkErr_Failed_Open_File(NULL, dl_file_path);
			// unreachable
		}
	}

	Ink_Package(Ink_MagicNumber magic, InkPack_Info *info, InkPack_FileBlock *so)
	: magic_num(magic), pack_info(info), dl_file(so)
	{ }
	
	void writeTo(FILE *fp)
	{
		fwrite(&magic_num, sizeof(Ink_MagicNumber), 1, fp);
		pack_info->writeTo(fp);
		dl_file->writeTo(fp);
		return;
	}

	static Ink_Package *readFrom(FILE *fp);
	static void preload(const char *path);

	~Ink_Package()
	{
		delete pack_info;
		delete dl_file;
	}
};

void Ink_addModule(INK_DL_HANDLER handler);
void Ink_disposeModules();
void Ink_preloadModule(const char *name);
void Ink_loadAllModules();
void Ink_applyAllModules(Ink_InterpreteEngine *engine, Ink_ContextChain *context);
inline bool createDirIfNotExist(const char *path); /* return: if exist */

#if defined(INK_PLATFORM_LINUX)
	inline bool
	createDirIfNotExist(const char *path)
	{
		if (!isDirExist(path)) {
			mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			return true;
		}
		return false;
	}
#elif defined(INK_PLATFORM_WIN32)
	#include <windows.h>
	#include <direct.h>

	inline bool
	createDirIfNotExist(const char *path)
	{
		if (!isDirExist(path)) {
			_mkdir(path);
			return true;
		}
		return false;
	}
#endif

#endif
