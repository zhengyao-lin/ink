#ifndef _PKG_LOAD_H_
#define _PKG_LOAD_H_

#include <string>
#include <vector>
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

void addHandler(void *handler);
void closeAllHandler();

#ifdef __linux__
	#include <sys/types.h>
	#include <dirent.h>
	#include <dlfcn.h>

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