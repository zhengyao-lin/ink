#include <string>
#include <stdlib.h>
#include "blueprint.h"
#include "error.h"
#include "core/object.h"
#include "core/native/general.h"
#include "includes/switches.h"

using namespace std;

extern char **environ;

Ink_Object *InkMod_Blueprint_System_Env_AtAssign(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	if (!checkArgument(engine, argc, 1)) {
		return NULL_OBJ;
	}

	if (argv[0]->type != INK_STRING) {
		InkWarn_Blueprint_Env_Need_String_Argument(engine, argv[0]->type);
		return NULL_OBJ;
	}

	if (!base->address) {
		InkWarn_Blueprint_Env_Not_A_Part_Of_Env(engine);
		return NULL_OBJ;
	}

	string val = as<Ink_String>(argv[0])->getValue();
	int err_code;

	if ((err_code = setenv(base->address->key, val.c_str(), true)) != 0) {
		InkWarn_Blueprint_Env_Failed_Set_Env(engine, base->address->key);
	}

	return NULL_OBJ;
}

Ink_Object *InkMod_Blueprint_System_Env_Missing(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	string name = as<Ink_String>(argv[0])->getValue();

	Ink_HashTable *addr = base->setSlot(name.c_str(), NULL);

	Ink_Object *ret = new Ink_Undefined(engine);
	ret->setSlot("@assign", new Ink_FunctionObject(engine, InkMod_Blueprint_System_Env_AtAssign));
	ret->address = addr;

	return ret;
}

Ink_Object *parseEnv(Ink_InterpreteEngine *engine)
{
	Ink_Object *ret = new Ink_Object(engine), *tmp;
	Ink_SizeType i;
	string::size_type j;
	string tmp_str;

	if (!environ)
		return ret;

	Ink_Object *at_assign = new Ink_FunctionObject(engine, InkMod_Blueprint_System_Env_AtAssign);

	ret->setSlot("missing", new Ink_FunctionObject(engine, InkMod_Blueprint_System_Env_Missing));
	for (i = 0; environ[i]; i++) {
		tmp_str = string(environ[i]);
		j = tmp_str.find_first_of('=');
		ret->setSlot(tmp_str.substr(0, j).c_str(),
					 tmp = new Ink_String(engine, tmp_str.substr(j + 1)));
		tmp->setSlot("@assign", at_assign);
	}

	return ret;
}

Ink_Object *InkMod_Blueprint_System_SetEnv(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 2, INK_STRING)) {
		return NULL_OBJ;
	}

	bool override = true;
	if (argc > 2 && argv[2]->type == INK_NUMERIC) {
		override = as<Ink_Numeric>(argv[2])->value;
	}

	string name = as<Ink_String>(argv[0])->getValue();
	string val = as<Ink_String>(argv[1])->getValue();

	return new Ink_Numeric(engine, setenv(name.c_str(), val.c_str(), override) == 0);
}

Ink_Object *InkMod_Blueprint_System_GetEnv(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	string name = as<Ink_String>(argv[0])->getValue();
	const char *val = getenv(name.c_str());

	return val ? (Ink_Object *)new Ink_String(engine, string(val)) : (Ink_Object *)NULL_OBJ;
}

Ink_Object *InkMod_Blueprint_System_Command(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	string cmd = as<Ink_String>(argv[0])->getValue();

	return new Ink_Numeric(engine, system(cmd.c_str()));
}

void InkMod_Blueprint_System_Path_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot("sep", new Ink_String(engine, INK_PATH_SPLIT));

	return;
}

Ink_Object *InkMod_Blueprint_System_Path_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *apply_to = argv[1];

	InkMod_Blueprint_System_Path_bondTo(engine, apply_to);

	return NULL_OBJ;
}

void InkMod_Blueprint_System_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot("env", parseEnv(engine));
	bondee->setSlot("setenv", new Ink_FunctionObject(engine, InkMod_Blueprint_System_SetEnv));
	bondee->setSlot("getenv", new Ink_FunctionObject(engine, InkMod_Blueprint_System_GetEnv));
	bondee->setSlot("cmd", new Ink_FunctionObject(engine, InkMod_Blueprint_System_Command));

	Ink_Object *path_pkg = addPackage(engine, bondee, "path",
									  new Ink_FunctionObject(engine, InkMod_Blueprint_System_Path_Loader));

	InkMod_Blueprint_System_Path_bondTo(engine, path_pkg);
	
	return;
}

Ink_Object *InkMod_Blueprint_System_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *apply_to = argv[1];

	InkMod_Blueprint_System_bondTo(engine, apply_to);

	return NULL_OBJ;
}
