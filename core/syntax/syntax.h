#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#include "../general.h"

namespace ink {

class Ink_InterpreteEngine;

void InkParser_setParserInfo(Ink_LineNoType lineno, const char *yyprefix);
Ink_LineNoType InkParser_getCurrentLineno();
const char *InkParser_getErrPrefix();

Ink_InterpreteEngine *InkParser_getParseEngine();
void InkParser_setParseEngine(Ink_InterpreteEngine *engine);
void InkParser_unlockParseLock();
void InkParser_lockParseLock();

}

#endif
