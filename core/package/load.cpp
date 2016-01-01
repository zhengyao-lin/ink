#include "load.h"

DLHandlerPool dl_handler_pool;

void addHandler(void *handler)
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