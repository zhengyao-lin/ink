#ifndef _PKG_LOAD_H_
#define _PKG_LOAD_H_

#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include "../context.h"
#include "../../includes/switches.h"
#include "../error.h"

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

typedef void (*InkMod_Loader)(Ink_ContextChain *context);
typedef vector<void *> DLHandlerPool;
typedef long int Ink_MagicNumber;
typedef size_t InkPack_Size;
typedef char byte;

void addHandler(void *handler);
void closeAllHandler();

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

	InkPack_Info(char *pack_name, char *author)
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
		fread(data, sizeof(byte) * file_size, 1, fp);
	}

	InkPack_FileBlock(InkPack_Size size, byte *d)
	: file_size(size), data(d)
	{ }

	void writeTo(FILE *fp)
	{
		fwrite(&file_size, sizeof(InkPack_Size), 1, fp);
		fwrite(data, sizeof(byte) * file_size, 1, fp);
	}

	string *bufferToTmp() // return: tmp file path
	{
		FILE *fp;
		char *suffix;
		int current_bit = 1;
		string path;

		suffix = (char *)malloc(sizeof(char) * (current_bit + 1));
		for (suffix[0] = 'a', suffix[1] = '\0';
			 !(fp = fopen((path = string(INK_TMP_PATH "/ink_tmp_") + string(suffix)).c_str(), "wbx"));) {
			int i = current_bit - 1;
			for (; i >= 0 && suffix[i] >= 'z'; i--) ;

			if (i < 0) {
				free(suffix);
				suffix = (char *)malloc(sizeof(char) * (++ current_bit + 1));
				for (i = 0; i < current_bit; i++)
					suffix[i] = 'a';
				suffix[i] = '\0';
			} else {
				suffix[i]++;
				for (i++; i < current_bit; i++)
					suffix[i] = 'a';
			}
		}
		free(suffix);
		fwrite(data, sizeof(byte) * file_size, 1, fp);
		fclose(fp);

		return new string(path);
	}

	static InkPack_FileBlock *readFrom(FILE *fp);

	~InkPack_FileBlock()
	{
		free(data);
	}
};

#ifdef __linux__
	#include <sys/types.h>
	#include <dirent.h>
	#include <dlfcn.h>
#endif

class Ink_Package {
public:
	Ink_MagicNumber magic_num;
	InkPack_Info *pack_info;
	InkPack_FileBlock *so_file;

	Ink_Package(char *pack_name, char *author, char *so_file_path)
	{
		FILE *fp;

		magic_num = 0;
		pack_info = new InkPack_Info(pack_name, author);
		fp = fopen(so_file_path, "rb");
		if (fp) {
			so_file = new InkPack_FileBlock(fp);
		} else {
			InkErr_Failed_Open_File(so_file_path);
			// unreachable
		}
	}

	Ink_Package(Ink_MagicNumber magic, InkPack_Info *info, InkPack_FileBlock *so)
	: magic_num(magic), pack_info(info), so_file(so)
	{ }
	
	void writeTo(FILE *fp)
	{
		fwrite(&magic_num, sizeof(Ink_MagicNumber), 1, fp);
		pack_info->writeTo(fp);
		so_file->writeTo(fp);
		return;
	}

	static Ink_Package *readFrom(FILE *fp);
	static void load(Ink_ContextChain *context, const char *path)
	{
		void *handler;
		FILE *fp = fopen(path, "rb");
		string *tmp;

		if (!fp) {
			InkErr_Failed_Open_File(path);
			// unreachable
		}

		Ink_Package *pack = Ink_Package::readFrom(fp);
		tmp = pack->so_file->bufferToTmp();
		handler = dlopen(tmp->c_str(), RTLD_NOW);
		delete tmp;
		InkMod_Loader loader = (InkMod_Loader)dlsym(handler, "InkMod_Loader");

		if (!handler || !loader) {
			InkWarn_Failed_Load_Mod(path);
			if (handler) dlclose(handler);
			printf("%s\n", dlerror());

			delete pack;
			fclose(fp);

			return;
		}
		loader(context);
		addHandler(handler);
		printf("Package Loader: Loading package: %s by %s\n",
			   pack->pack_info->pack_name->str,
			   pack->pack_info->author->str);
		delete pack;
		fclose(fp);

		return;
	}

	~Ink_Package()
	{
		delete pack_info;
		delete so_file;
	}
};

#ifdef __linux__
	inline void loadAllModules(Ink_ContextChain *context)
	{
		DIR *mod_dir = opendir(INK_MODULE_DIR);
		struct dirent *child;
		void *handler;

		if (!mod_dir) {
			InkWarn_Failed_Find_Mod(INK_MODULE_DIR);
			return;
		}

		while ((child = readdir(mod_dir)) != NULL) {
			if (hasSuffix(child->d_name, "mod")) {
				Ink_Package::load(context, (string(INK_MODULE_DIR) + string("/") + string(child->d_name)).c_str());
			} else if (hasSuffix(child->d_name, "so")) {
				handler = dlopen((string(INK_MODULE_DIR) + string("/") + string(child->d_name)).c_str(), RTLD_NOW);
				InkMod_Loader loader = (InkMod_Loader)dlsym(handler, "InkMod_Loader");
				if (!handler || !loader) {
					InkWarn_Failed_Load_Mod(child->d_name);
					if (handler) dlclose(handler);
					printf("%s\n", dlerror());
				} else {
					loader(context);
					addHandler(handler);
				}
			}
		}

		closedir(mod_dir);

		return;
	}
#endif

#endif