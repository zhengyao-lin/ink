#ifndef _PACK_MOD_H_
#define _PACK_MOD_H_

#include <string>
#include "general.h"
#include "../error.h"
#include "../inttype.h"
#include "../../includes/universal.h"

namespace ink {

class Ink_InterpreteEngine;
class Ink_ContextChain;

typedef long InkMod_ModuleID;
typedef void (*InkMod_Loader_t)(Ink_InterpreteEngine *engine, Ink_ContextChain *context);
typedef int (*InkMod_Init_t)(InkMod_ModuleID id);
typedef size_t InkPack_Size;
typedef Ink_Byte byte;

#define INKPACK_SIZE_INVALID ((InkPack_Size)-1)

enum Ink_MagicNumber {
	INK_MAGIC_NUM_INVALID = 0,
	INK_MAGIC_NUM_0_LINUX,
	INK_MAGIC_NUM_0_WINDOWS
};

#if defined(INK_PLATFORM_LINUX)
	#define INK_DEFAULT_MAGIC_NUM INK_MAGIC_NUM_0_LINUX
#elif defined(INK_PLATFORM_WIN32)
	#define INK_DEFAULT_MAGIC_NUM INK_MAGIC_NUM_0_WINDOWS
#endif

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

	inline void writeTo(FILE *fp)
	{
		fwrite(&len, sizeof(InkPack_Size), 1, fp);
		fwrite(str, sizeof(const char), len, fp);
	}

	static InkPack_String *readFrom(FILE *fp);

	~InkPack_String()
	{
		free(str);
	}
};

template <typename T_T1, typename T_T2>
class InkPack_Table {
public:
	InkPack_Size size;
	struct InkPack_Table_sub {
		T_T1 key;
		T_T2 value;

		InkPack_Table_sub(T_T1 key, T_T2 value)
		: key(key), value(value)
		{ }
	} *table;

	InkPack_Table()
	{
		size = 0;
		table = NULL;
	}

	InkPack_Table(InkPack_Size size, InkPack_Table_sub *table)
	: size(size), table(table)
	{ }

	inline void writeTo(FILE *fp)
	{
		fwrite(&size, sizeof(InkPack_Size), 1, fp);

		InkPack_Size i;

		for (i = 0; i < size; i++) {
			fwrite(&(table[i].key), sizeof(T_T1), 1, fp);
			fwrite(&(table[i].value), sizeof(T_T2), 1, fp);
		}
	}

	static InkPack_Table<T_T1, T_T2> *readFrom(FILE *fp)
	{
		InkPack_Size size = 0;
		InkPack_Table_sub *table = NULL;

		if (!fread(&size, sizeof(InkPack_Size), 1, fp))
			return NULL;

		table = (InkPack_Table_sub *)malloc(sizeof(InkPack_Table_sub) * size);

		InkPack_Size i;
		for (i = 0; i < size; i++) {
			if (!(fread(&(table[i].key), sizeof(T_T1), 1, fp)
				&& fread(&(table[i].value), sizeof(T_T2), 1, fp))) {
				free(table);
				return NULL;
			}
		}

		return new InkPack_Table<T_T1, T_T2>(size, table);
	}

	inline void addTable(T_T1 k, T_T2 v)
	{
		table = (InkPack_Table_sub *)realloc(table, sizeof(InkPack_Table_sub) * (++size));
		table[size - 1] = InkPack_Table_sub(k, v);
		return;
	}

	inline T_T2 findValueByKey(T_T1 k, T_T2 default_val = NULL)
	{
		InkPack_Size i;
		for (i = 0; i < size; i++) {
			if (k == table[i].key) {
				return table[i].value;
			}
		}
		return default_val;
	}

	inline T_T1 findKeyByValue(T_T2 v, T_T1 default_val = NULL)
	{
		InkPack_Size i;
		for (i = 0; i < size; i++) {
			if (v == table[i].value) {
				return table[i].key;
			}
		}
		return default_val;
	}

	~InkPack_Table()
	{
		free(table);
	}
};

class InkPack_Info {
	typedef InkPack_Table<Ink_MagicNumber, InkPack_Size> InkPack_VersionTable;
public:
	InkPack_String *pack_name;
	InkPack_String *author;
	InkPack_VersionTable *ver_table;

	InkPack_Info(const char *pack_name, const char *author)
	: pack_name(new InkPack_String(pack_name)), author(new InkPack_String(author)),
	  ver_table(new InkPack_VersionTable())
	{ }

	InkPack_Info(InkPack_String *pack_name, InkPack_String *author,
				 InkPack_VersionTable *ver_table)
	: pack_name(pack_name), author(author), ver_table(ver_table)
	{ }

	inline void addVersion(Ink_MagicNumber vers, InkPack_Size index)
	{
		ver_table->addTable(vers, index);
		return;
	}

	inline Ink_MagicNumber getVersionByIndex(InkPack_Size index)
	{
		return ver_table->findKeyByValue(index, INK_MAGIC_NUM_INVALID);
	}

	inline InkPack_Size findVersion(Ink_MagicNumber vers)
	{
		return ver_table->findValueByKey(vers, INKPACK_SIZE_INVALID);
	}

	inline void writeTo(FILE *fp)
	{
		pack_name->writeTo(fp);
		author->writeTo(fp);
		ver_table->writeTo(fp);
	}

	static InkPack_Info *readFrom(FILE *fp);

	~InkPack_Info()
	{
		delete pack_name;
		delete author;
		delete ver_table;
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
		if (!fread(data, sizeof(byte), file_size, fp)) {
			free(data);
			file_size = 0;
			data = NULL;
		}
	}

	InkPack_FileBlock(InkPack_Size size, byte *d)
	: file_size(size), data(d)
	{ }

	inline void writeTo(FILE *fp)
	{
		fwrite(&file_size, sizeof(InkPack_Size), 1, fp);
		fwrite(data, sizeof(byte) * file_size, 1, fp);
	}

	std::string *bufferToTmp(const char *file_suffix = "." INK_DL_SUFFIX); // return: tmp file path
	static InkPack_FileBlock *readFrom(FILE *fp);

	~InkPack_FileBlock()
	{
		free(data);
	}
};

class Ink_Package {
public:
	InkPack_Info *pack_info;

	InkPack_Size dl_file_count;
	InkPack_FileBlock **dl_file;

	Ink_Package(const char *pack_name, const char *author, const char *dl_file_path = NULL, Ink_MagicNumber magic_num = INK_DEFAULT_MAGIC_NUM)
	{
		pack_info = new InkPack_Info(pack_name, author);
		dl_file_count = 0;
		dl_file = NULL;
		
		if (dl_file_path)
			addDLFile(dl_file_path, magic_num);
	}

	Ink_Package(InkPack_Info *info)
	: pack_info(info), dl_file_count(0), dl_file(NULL)
	{ }

	inline InkPack_FileBlock *addDLFile(InkPack_FileBlock *file, Ink_MagicNumber vers)
	{
		dl_file = (InkPack_FileBlock **)realloc(dl_file, sizeof(InkPack_FileBlock *) * (++dl_file_count));
		dl_file[dl_file_count - 1] = file;
		pack_info->addVersion(vers, dl_file_count - 1);
		return file;
	}

	inline InkPack_FileBlock *addDLFile(const char *dl_file_path, Ink_MagicNumber vers)
	{
		FILE *fp = fopen(dl_file_path, "rb");

		if (fp) {
			dl_file = (InkPack_FileBlock **)realloc(dl_file, sizeof(InkPack_FileBlock *) * (++dl_file_count));
			dl_file[dl_file_count - 1] = new InkPack_FileBlock(fp);
			pack_info->addVersion(vers, dl_file_count - 1);
			return dl_file[dl_file_count - 1];
		} else {
			InkError_Failed_Open_File(NULL, dl_file_path);
		}

		return NULL;
	}
	
	inline void writeTo(FILE *fp)
	{
		pack_info->writeTo(fp);
		fwrite(&dl_file_count, sizeof(InkPack_Size), 1, fp);

		InkPack_Size i;
		for (i = 0; i < dl_file_count; i++) {
			dl_file[i]->writeTo(fp);
		}
		return;
	}

	static Ink_Package *readFrom(FILE *fp);
	static void preload(const char *path);

	~Ink_Package()
	{
		delete pack_info;
		InkPack_Size i;
		for (i = 0; i < dl_file_count; i++)
			delete dl_file[i];
		free(dl_file);
	}
};

}

#endif
