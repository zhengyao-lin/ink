#include <stdio.h>
#include "load.h"

DLHandlerPool dl_handler_pool;

void addHandler(INK_DL_HANDLER handler)
{
	dl_handler_pool.push_back(handler);
	return;
}

void closeAllHandler()
{
	unsigned int i;
	for (i = 0; i < dl_handler_pool.size(); i++) {
		dlclose(dl_handler_pool[i]);
	}
	return;
}

InkPack_String *InkPack_String::readFrom(FILE *fp)
{
	InkPack_Size tmp_len;
	char *tmp_str;

	fread(&tmp_len, sizeof(InkPack_Size), 1, fp);
	tmp_str = (char *)malloc(sizeof(char) * (tmp_len + 1));
	fread(tmp_str, sizeof(char) * (tmp_len + 1), 1, fp);

	return new InkPack_String(tmp_str, false);
}

InkPack_Info *InkPack_Info::readFrom(FILE *fp)
{
	InkPack_String *pack_name = InkPack_String::readFrom(fp);
	InkPack_String *author = InkPack_String::readFrom(fp);
	return new InkPack_Info(pack_name, author);
}

InkPack_FileBlock *InkPack_FileBlock::readFrom(FILE *fp)
{
	InkPack_Size tmp_size;
	byte *data;

	fread(&tmp_size, sizeof(InkPack_Size), 1, fp);
	data = (byte *)malloc(sizeof(byte) * tmp_size);
	fread(data, sizeof(byte) * tmp_size, 1, fp);

	return new InkPack_FileBlock(tmp_size, data);
}

Ink_Package *Ink_Package::readFrom(FILE *fp)
{
	Ink_MagicNumber tmp_magic_num;
	fread(&tmp_magic_num, sizeof(Ink_MagicNumber), 1, fp);
	InkPack_Info *tmp_pack_info = InkPack_Info::readFrom(fp);
	InkPack_FileBlock *tmp_so_file = InkPack_FileBlock::readFrom(fp);
	return new Ink_Package(tmp_magic_num, tmp_pack_info, tmp_so_file);
}

string *InkPack_FileBlock::bufferToTmp() // return: tmp file path
{
	FILE *fp;
	char *suffix;
	int current_bit = 1;
	string path;

#ifdef __linux__
	if (!isDirExist(INK_TMP_PATH))
		mkdir(INK_TMP_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

	suffix = (char *)malloc(sizeof(char) * (current_bit + 1));
	for (suffix[0] = 'a', suffix[1] = '\0';
		 !(fp = fopen((path = string(INK_TMP_PATH INK_PATH_SPLIT) + string(suffix)).c_str(), "wbx"));) {
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

void Ink_Package::load(Ink_ContextChain *context, const char *path)
{
#ifndef INK_STATIC
	INK_DL_HANDLER handler;
	FILE *fp = fopen(path, "rb");
	string *tmp;

	if (!fp) {
		InkErr_Failed_Open_File(path);
		// unreachable
	}

	Ink_Package *pack = Ink_Package::readFrom(fp);
	tmp = pack->so_file->bufferToTmp();
	handler = INK_DL_OPEN(tmp->c_str(), RTLD_NOW);
	delete tmp;
	InkMod_Loader loader = (InkMod_Loader)INK_DL_SYMBOL(handler, "InkMod_Loader");

	if (!handler || !loader) {
		InkWarn_Failed_Load_Mod(path);
		if (handler) INK_DL_CLOSE(handler);
		printf("%s\n", INK_DL_ERROR());

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
#endif

	return;
}