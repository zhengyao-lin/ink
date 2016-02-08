#include <string>
#include <stdio.h>
#include "load.h"
#include "../thread/thread.h"
#include "../interface/engine.h"
#include "../../includes/switches.h"

namespace ink {

using namespace std;

static pthread_mutex_t dl_handler_pool_lock = PTHREAD_MUTEX_INITIALIZER;
static DLHandlerPool dl_handler_pool;
static char *tmp_prog_path = NULL;
static InkMod_ModuleID current_module_id = 0;

void Ink_addModule(INK_DL_HANDLER handler)
{
	pthread_mutex_lock(&dl_handler_pool_lock);
	dl_handler_pool.push_back(handler);
	pthread_mutex_unlock(&dl_handler_pool_lock);
	return;
}

void Ink_disposeModules()
{
	DLHandlerPool::size_type i;
	pthread_mutex_lock(&dl_handler_pool_lock);
	for (i = 0; i < dl_handler_pool.size(); i++) {
		INK_DL_CLOSE(dl_handler_pool[i]);
	}
	if (tmp_prog_path) {
		free(tmp_prog_path);
	}
	pthread_mutex_unlock(&dl_handler_pool_lock);
	return;
}

void Ink_applyAllModules(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
{
	DLHandlerPool::iterator iter;

	pthread_mutex_lock(&dl_handler_pool_lock);
	for (iter = dl_handler_pool.begin();
		 iter != dl_handler_pool.end(); iter++) {
		if (*iter) {
			InkMod_Loader_t loader = (InkMod_Loader_t)INK_DL_SYMBOL(*iter, "InkMod_Loader");
			if (loader)
				loader(engine, context);
		}
	}
	pthread_mutex_unlock(&dl_handler_pool_lock);

	return;
}

InkMod_ModuleID registerModule()
{
	return ++current_module_id;
}

InkPack_String *InkPack_String::readFrom(FILE *fp)
{
	InkPack_Size tmp_len;
	char *tmp_str;

	if (!fread(&tmp_len, sizeof(InkPack_Size), 1, fp)) {
		return NULL;
	}
	tmp_str = (char *)malloc(sizeof(char) * (tmp_len + 1));
	if (!fread(tmp_str, sizeof(char), tmp_len + 1, fp)) {
		return NULL;
	}

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

	fread(&dl_file_count, sizeof(InkPack_Size), 1, fp);

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

void Ink_Package::preload(const char *path)
{
	INK_DL_HANDLER handler;
	FILE *fp = fopen(path, "rb");
	string *tmp;
	int errnum;
	const char *errmsg;
	InkPack_Size index;

	if (!fp) {
		InkError_Failed_Open_File(NULL, path);
		// unreachable
	}

	Ink_Package *pack = Ink_Package::readFrom(fp);

	if ((index = pack->pack_info->findVersion(INK_DEFAULT_MAGIC_NUM))
		== INKPACK_SIZE_INVALID) {
		InkWarn_Load_Mod_On_Wrong_OS(NULL, path);
		delete pack;
		fclose(fp);
		return;
	}

	if (!pack->dl_file) {
		InkWarn_No_File_In_Mod(NULL, path);
		delete pack;
		fclose(fp);
		return;
	}

	tmp = pack->dl_file[index]->bufferToTmp();
	handler = INK_DL_OPEN(tmp->c_str(), RTLD_NOW);
	delete tmp;
	InkMod_Loader_t loader = (InkMod_Loader_t)INK_DL_SYMBOL(handler, "InkMod_Loader");
	InkMod_Init_t init = (InkMod_Init_t)INK_DL_SYMBOL(handler, "InkMod_Init");

	if (!handler) {
		InkWarn_Failed_Load_Mod(NULL, path);
		if ((errmsg = INK_DL_ERROR()) != NULL)
			printf("%s\n", errmsg);

		delete pack;
		fclose(fp);
		return;
	}

	if (!loader) {
		InkWarn_Failed_Find_Loader(NULL, path);
		INK_DL_CLOSE(handler);

		if ((errmsg = INK_DL_ERROR()) != NULL)
			printf("%s\n", errmsg);

		delete pack;
		fclose(fp);
		return;
	}

	if (!init) {
		InkWarn_Failed_Find_Init(NULL, path);
		INK_DL_CLOSE(handler);

		if ((errmsg = INK_DL_ERROR()) != NULL)
			printf("%s\n", errmsg);

		delete pack;
		fclose(fp);
		return;
	}

	if ((errnum = init(registerModule())) != 0) {
		InkWarn_Failed_Init_Mod(NULL, errnum);
		INK_DL_CLOSE(handler);
		delete pack;
		fclose(fp);
		return;
	}

	Ink_addModule(handler);
	printf("Package Loader: Loading package: %s by %s\n",
		   pack->pack_info->pack_name->str,
		   pack->pack_info->author->str);
	delete pack;
	fclose(fp);

	return;
}

void Ink_preloadModule(const char *name)
{
	INK_DL_HANDLER handler = INK_DL_OPEN((string(INK_MODULE_DIR) + INK_PATH_SPLIT + string(name)).c_str(), RTLD_NOW);
	int errnum;
	const char *errmsg;

	if (!handler) {
		InkWarn_Failed_Load_Mod(NULL, name);
		if ((errmsg = INK_DL_ERROR()) != NULL)
			printf("%s\n", errmsg);
		return;
	}

	InkMod_Loader_t loader = (InkMod_Loader_t)INK_DL_SYMBOL(handler, "InkMod_Loader");
	InkMod_Init_t init = (InkMod_Init_t)INK_DL_SYMBOL(handler, "InkMod_Init");
	if (!loader) {
		InkWarn_Failed_Find_Loader(NULL, name);
		INK_DL_CLOSE(handler);
		if ((errmsg = INK_DL_ERROR()) != NULL)
			printf("%s\n", errmsg);
		return;
	}

	if (!init) {
		InkWarn_Failed_Find_Init(NULL, name);
		INK_DL_CLOSE(handler);
		if ((errmsg = INK_DL_ERROR()) != NULL)
			printf("%s\n", errmsg);
		return;
	}

	if ((errnum = init(registerModule())) != 0) {
		InkWarn_Failed_Init_Mod(NULL, errnum);
		INK_DL_CLOSE(handler);
		return;
	}
	
	Ink_addModule(handler);
	return;
}

#if defined(INK_PLATFORM_LINUX)
	void Ink_loadAllModules()
	{
		DIR *mod_dir = opendir(INK_MODULE_DIR);
		struct dirent *child;

		if (!mod_dir) {
			InkWarn_Failed_Find_Mod(NULL, INK_MODULE_DIR);
			return;
		}

		while ((child = readdir(mod_dir)) != NULL) {
			if (hasSuffix(child->d_name, "mod")) {
				Ink_Package::preload((string(INK_MODULE_DIR) + INK_PATH_SPLIT + child->d_name).c_str());
			} else if (hasSuffix(child->d_name, INK_DL_SUFFIX)) {
				Ink_preloadModule(child->d_name);
			}
		}

		closedir(mod_dir);
		
		return;
	}
#elif defined(INK_PLATFORM_WIN32)
	void Ink_loadAllModules()
	{
		WIN32_FIND_DATA data;
		HANDLE dir_handle = NULL;

		dir_handle = FindFirstFile((string(INK_MODULE_DIR) + "/*").c_str(), &data);  // find for all files
		if (dir_handle == INVALID_HANDLE_VALUE) {
			InkWarn_Failed_Find_Mod(NULL, INK_MODULE_DIR);
			return;
		}

		do {
			if (hasSuffix(data.cFileName, "mod")) {
				Ink_Package::preload((string(INK_MODULE_DIR) + INK_PATH_SPLIT + string(data.cFileName)).c_str());
			} else if (hasSuffix(data.cFileName, INK_DL_SUFFIX)) {
				Ink_preloadModule(data.cFileName);
			}
		} while (FindNextFile(dir_handle, &data));

		FindClose(dir_handle);
	}
#endif

#if defined(INK_PLATFORM_WIN32)
	string getProgPath()
	{
		if (tmp_prog_path) return string(tmp_prog_path);
		char buffer[MAX_PATH + 1];
		GetModuleFileName(NULL, buffer, MAX_PATH);
		return string(tmp_prog_path = getBasePath(buffer));
	}
#endif

}