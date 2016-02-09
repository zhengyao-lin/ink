#include "syntax.h"
#include "../interface/engine.h"

static ink::Ink_InterpreteEngine *ink_parse_engine = NULL;
static pthread_mutex_t ink_parse_lock = PTHREAD_MUTEX_INITIALIZER;

namespace ink {

Ink_InterpreteEngine *InkParser_getParseEngine()
{
	return ink_parse_engine;
}

void InkParser_setParseEngine(Ink_InterpreteEngine *engine)
{
	ink_parse_engine = engine;
	return;
}

void InkParser_lockParseLock()
{
	pthread_mutex_lock(&ink_parse_lock);
	return;
}

void InkParser_unlockParseLock()
{
	pthread_mutex_unlock(&ink_parse_lock);
	return;
}

}
