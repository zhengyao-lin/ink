#include "module.h"

namespace ink {

using namespace std;

InkPack_String *InkPack_String::readFrom(FILE *fp)
{
	InkPack_Size tmp_len;
	char *tmp_str;

	if (!fread(&tmp_len, sizeof(InkPack_Size), 1, fp)) {
		return NULL;
	}
	tmp_str = (char *)malloc(sizeof(char) * (tmp_len + 1));
	if (!fread(tmp_str, sizeof(const char), tmp_len, fp)) {
		return NULL;
	}
	if (tmp_len)
		tmp_str[tmp_len] = '\0';
	printf("len: %ld, str: %s\n", tmp_len, tmp_str);

	return new InkPack_String(tmp_str, false);
}

InkPack_Info *InkPack_Info::readFrom(FILE *fp)
{
	InkPack_String *pack_name = InkPack_String::readFrom(fp);
	InkPack_String *author = InkPack_String::readFrom(fp);
	InkPack_VersionTable *ver_table = InkPack_VersionTable::readFrom(fp);
	return new InkPack_Info(pack_name, author, ver_table);
}

InkPack_FileBlock *InkPack_FileBlock::readFrom(FILE *fp)
{
	InkPack_Size tmp_size;
	byte *data;

	if (!fread(&tmp_size, sizeof(InkPack_Size), 1, fp))
		return NULL;
	data = (byte *)malloc(sizeof(byte) * tmp_size);
	if (!fread(data, sizeof(byte), tmp_size, fp))
		return NULL;

	return new InkPack_FileBlock(tmp_size, data);
}

Ink_Package *Ink_Package::readFrom(FILE *fp)
{
	InkPack_Info *tmp_pack_info = InkPack_Info::readFrom(fp);
	if (!tmp_pack_info)
		return NULL;
	InkPack_Size i, dl_file_count = 0;
	Ink_Package *ret = new Ink_Package(tmp_pack_info);
	InkPack_FileBlock *tmp = NULL;

	if (!fread(&dl_file_count, sizeof(InkPack_Size), 1, fp)) {
		delete ret;
		return NULL;
	}

	for (i = 0; i < dl_file_count; i++) {
		ret->addDLFile(tmp = InkPack_FileBlock::readFrom(fp),
					   tmp_pack_info->getVersionByIndex(i));
		if (!tmp) {
			delete ret;
			return NULL;
		}
	}

	return ret;
}

string *InkPack_FileBlock::bufferToTmp(const char *file_suffix) // return: tmp file path
{
	FILE *fp;
	char *suffix;
	int current_bit = 1;
	string path;

	createDirIfNotExist(INK_TMP_PATH);
	suffix = (char *)malloc(sizeof(char) * (current_bit + 1));
	suffix[0] = 'a';
	suffix[1] = '\0';

	path = string(INK_TMP_PATH) + INK_PATH_SPLIT + string(suffix) + file_suffix;

	while (!access(path.c_str(), 0)) { // file exist
		int i = current_bit - 1;
		for (; i >= 0 && suffix[i] >= 'z'; i--) ;

		if (i < 0) {
			free(suffix);
			suffix = (char *)malloc(sizeof(char) * (++current_bit + 1));
			for (i = 0; i < current_bit; i++)
				suffix[i] = 'a';
			suffix[i] = '\0';
		} else {
			suffix[i]++;
			for (i++; i < current_bit; i++)
				suffix[i] = 'a';
		}
		path = string(INK_TMP_PATH) + INK_PATH_SPLIT + string(suffix) + file_suffix;
	}
	free(suffix);
	fp = fopen(path.c_str(), "wb");
	fwrite(data, sizeof(byte), file_size, fp);
	fclose(fp);
	return new string(path);
}

}
