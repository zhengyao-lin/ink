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