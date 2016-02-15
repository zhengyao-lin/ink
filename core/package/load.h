#ifndef _PKG_LOAD_H_
#define _PKG_LOAD_H_

#include <vector>
#include "general.h"
#include "module.h"
#include "../inttype.h"
#include "../../includes/universal.h"

namespace ink {

#if defined(INK_PLATFORM_LINUX)

	#ifdef INK_INSTALL_PATH
		#define INK_MODULE_DIR INK_INSTALL_PATH "/lib/ink/modules"
	#else
		#define INK_MODULE_DIR "/usr/lib/ink/modules"
	#endif

#elif defined(INK_PLATFORM_WIN32)

	#include <string>
	#include <string.h>
	#include <windows.h>
	#define INK_MODULE_DIR ((getProgPath() + "modules").c_str())

	std::string getProgPath();

#endif

#if defined(INK_PLATFORM_LINUX)

	#include <sys/types.h>
	#include <dirent.h>
	#include <dlfcn.h>

	#define INK_DL_HANDLER void *
	#define INK_DL_OPEN(path, p) (dlopen((path), (p)))
	#define INK_DL_SYMBOL(handler, name) (dlsym((handler), (name)))
	#define INK_DL_CLOSE(handler) (dlclose(handler))
	#define INK_DL_ERROR() (dlerror())

#elif defined(INK_PLATFORM_WIN32)

	#include <windows.h>

	#define INK_DL_HANDLER HINSTANCE
	#define INK_DL_OPEN(path, p) (LoadLibrary(path))
	#define INK_DL_SYMBOL(handler, name) (GetProcAddress((handler), (name)))
	#define INK_DL_CLOSE(handler) (FreeLibrary(handler))
	#define INK_DL_ERROR() (NULL)
	
#endif

class Ink_InterpreteEngine;
class Ink_ContextChain;

typedef std::vector<INK_DL_HANDLER> DLHandlerPool;

void Ink_addModPath(const char *path);
void Ink_addModule(INK_DL_HANDLER handler);
void Ink_disposeModules();
void Ink_preloadModule(const char *name);
void Ink_loadAllModules();
void Ink_applyAllModules(Ink_InterpreteEngine *engine, Ink_ContextChain *context);

}

#endif
