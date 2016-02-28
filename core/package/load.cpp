#include <string>
#include <string.h>
#include "load.h"
#include "../error.h"
#include "../thread/thread.h"
#include "../interface/engine.h"
#include "../../includes/switches.h"

namespace ink {

using namespace std;

static pthread_mutex_t dl_handler_pool_lock = PTHREAD_MUTEX_INITIALIZER;
static DLHandlerPool dl_handler_pool;
static char *tmp_prog_path = NULL;
static char *tmp_module_path = NULL;
static InkMod_ModuleID current_module_id = 0;

static const char *dl_fixed_mod_load_dir[] = {
	INK_MODULE_DIR,
	NULL
};

static Ink_SizeType dl_mod_load_dir_len = 0;
static char **dl_mod_load_dir = NULL;

void Ink_addModPath(const char *path)
{
	dl_mod_load_dir = (char **)realloc(dl_mod_load_dir,
									   sizeof(char *) * (++dl_mod_load_dir_len));
	dl_mod_load_dir[dl_mod_load_dir_len - 1] = strdup(path);
	return;
}

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
	Ink_SizeType j;
	
	pthread_mutex_lock(&dl_handler_pool_lock);

	for (i = 0; i < dl_handler_pool.size(); i++) {
		INK_DL_CLOSE(dl_handler_pool[i]);
	}
	
	for (j = 0; j < dl_mod_load_dir_len; j++) {
		free(dl_mod_load_dir[j]);
	}
	free(dl_mod_load_dir);

	free(tmp_prog_path);
	free(tmp_module_path);
	
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

inline void showDLError()
{
	const char *errmsg;
	if ((errmsg = INK_DL_ERROR()) != NULL)
		printf("%s\n", errmsg);
	return;
}

void Ink_preloadModule(const char *path)
{
	INK_DL_HANDLER handler = INK_DL_OPEN(path, RTLD_NOW);
	int errnum;

	if (!handler) {
		InkWarn_Failed_Load_Mod(NULL, path);
		showDLError();
		return;
	}
	showDLError();

	InkMod_Loader_t loader = (InkMod_Loader_t)INK_DL_SYMBOL(handler, "InkMod_Loader");
	InkMod_Init_t init = (InkMod_Init_t)INK_DL_SYMBOL(handler, "InkMod_Init");
	if (!loader) {
		InkWarn_Failed_Find_Loader(NULL, path);
		INK_DL_CLOSE(handler);
		showDLError();
		return;
	}

	if (!init) {
		InkWarn_Failed_Find_Init(NULL, path);
		INK_DL_CLOSE(handler);
		showDLError();
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
	void Ink_loadAllModules_sub(const char *mod_path)
	{
		DIR *mod_dir = opendir(mod_path);
		struct dirent *child;

		if (!mod_dir) {
			InkWarn_Failed_Find_Mod(NULL, mod_path);
			return;
		}

		while ((child = readdir(mod_dir)) != NULL) {
			if (hasSuffix(child->d_name, INK_MOD_SUFFIX)) {
				Ink_Package::preload((string(mod_path) + INK_PATH_SPLIT + child->d_name).c_str());
			} else if (hasSuffix(child->d_name, INK_DL_SUFFIX)) {
				Ink_preloadModule((string(mod_path) + INK_PATH_SPLIT + child->d_name).c_str());
			}
		}

		closedir(mod_dir);
		
		return;
	}
#elif defined(INK_PLATFORM_WIN32)
	void Ink_loadAllModules_sub(const char *mod_path)
	{
		WIN32_FIND_DATA data;
		HANDLE dir_handle = NULL;

		dir_handle = FindFirstFile((string(mod_path) + "/*").c_str(), &data);  // find for all files
		if (dir_handle == INVALID_HANDLE_VALUE) {
			InkWarn_Failed_Find_Mod(NULL, mod_path);
			return;
		}

		do {
			if (hasSuffix(data.cFileName, INK_MOD_SUFFIX)) {
				Ink_Package::preload((string(mod_path) + INK_PATH_SPLIT + string(data.cFileName)).c_str());
			} else if (hasSuffix(data.cFileName, INK_DL_SUFFIX)) {
				Ink_preloadModule((string(mod_path) + INK_PATH_SPLIT + string(data.cFileName)).c_str());
			}
		} while (FindNextFile(dir_handle, &data));

		FindClose(dir_handle);
	}
#endif

void Ink_loadAllModules()
{
	Ink_SizeType i;

	for (i = 0; dl_fixed_mod_load_dir[i]; i++) {
		Ink_loadAllModules_sub(dl_fixed_mod_load_dir[i]);
	}

	for (i = 0; i < dl_mod_load_dir_len; i++) {
		Ink_loadAllModules_sub(dl_mod_load_dir[i]);
	}

	return;
}

#if defined(INK_PLATFORM_WIN32)
	string getProgPath()
	{
		if (tmp_prog_path) return string(tmp_prog_path);
		char buffer[MAX_PATH + 1];
		GetModuleFileName(NULL, buffer, MAX_PATH);
		return string(tmp_prog_path = getBasePath(buffer));
	}

	char *getModulePath()
	{
		if (tmp_module_path) return tmp_module_path;
		return tmp_module_path = strdup((getProgPath() + "/modules").c_str());
	}
#endif

}
