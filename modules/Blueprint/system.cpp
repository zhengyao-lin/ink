#include <string>
#include "blueprint.h"
#include "error.h"
#include "core/object.h"
#include "core/native/general.h"

using namespace std;

extern char **environ;

Ink_Object *parseEnv(Ink_InterpreteEngine *engine)
{
	Ink_Object *ret = new Ink_Object(engine);
	Ink_SizeType i;
	string::size_type j;
	string tmp_str;

	if (!environ)
		return ret;

	for (i = 0; environ[i]; i++) {
		tmp_str = string(environ[i]);
		j = tmp_str.find_first_of('=');
		ret->setSlot(tmp_str.substr(0, j).c_str(),
					 new Ink_String(engine, tmp_str.substr(j + 1)));
	}

	return ret;
}

void InkMod_Blueprint_System_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot("env", parseEnv(engine));
	
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
