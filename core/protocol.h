#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <map>
#include <string>
#include "general.h"

namespace ink {

class Ink_FunctionObject;
class Ink_InterpreteEngine;
class Ink_ContextChain;

typedef Ink_FunctionObject *(*Ink_Protocol)(Ink_InterpreteEngine *engine, Ink_ParamList param,
											Ink_ExpressionList exp_list, Ink_ContextChain *closure_context);
typedef std::map<std::string, Ink_Protocol> Ink_ProtocolMap;

}

#endif
