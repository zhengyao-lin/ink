#ifndef _NATIVE_GENERAL_H_
#define _NATIVE_GENERAL_H_

#include "../object.h"
#include "../expression.h"
#include "../context.h"
#include "../error.h"
#include "../general.h"

#define ASSUME_BASE_TYPE(eng, t) do { \
	if (!assumeType(eng, base, t)) { \
		return NULL_OBJ; \
	} \
} while (0)

namespace ink {

inline Ink_Object *getSlotWithProto(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
									Ink_Object *base, const char *name)
{
	return Ink_HashExpression::getSlot(engine, context, base, name);
}

inline Ink_Object *searchContextSlot(Ink_InterpreteEngine *engine, Ink_ContextChain *context, const char *name)
{
	return Ink_IdentifierExpression::getContextSlot(engine, context, name, Ink_EvalFlag(), false);
}

inline Ink_Object *callMethod(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, const char *method_name,
							  Ink_ArgcType argc = 0, Ink_Object **argv = NULL, Ink_Object *this_p = NULL)
{
	if ((base = getSlotWithProto(engine, context, base, method_name))->type != INK_FUNCTION) {
		InkWarn_Failed_Finding_Method(engine, method_name);
		return NULL;
	}
	return base->call(engine, context, argc, argv, this_p);
}

inline bool assumeType(Ink_InterpreteEngine *engine, Ink_Object *obj, Ink_TypeTag type_tag)
{
	if (!obj) return false;
	if (obj->type != type_tag) {
		InkWarn_Wrong_Type(engine, type_tag, obj->type);
		return false;
	}

	return true;
}

inline bool checkArgument(Ink_InterpreteEngine *engine, Ink_ArgcType argc, Ink_ArgcType min)
{
	if (argc < min) {
		InkWarn_Too_Less_Argument(engine, min, argc);
		return false;
	}
	return true;
}

inline bool checkArgument(bool if_output, Ink_ArgcType argc, Ink_Object **argv,
						  Ink_ArgcType min, Ink_TypeTag type1)
{
	if (argc < min) {
		return false;
	}

	if (argv[0]->type != type1) {
		return false;
	}

	return true;
}

inline bool checkArgument(Ink_InterpreteEngine *engine, Ink_ArgcType argc, Ink_Object **argv,
						  Ink_ArgcType min, Ink_TypeTag type1)
{
	if (argc < min) {
		InkWarn_Too_Less_Argument(engine, min, argc);
		return false;
	}

	if (argv[0]->type != type1) {
		InkWarn_Wrong_Argument_Type(engine, type1, argv[0]->type);
		return false;
	}

	return true;
}

inline bool checkArgument(Ink_InterpreteEngine *engine, Ink_ArgcType argc, Ink_Object **argv,
						  Ink_ArgcType min, Ink_TypeTag type1, Ink_TypeTag type2)
{
	if (argc < min) {
		InkWarn_Too_Less_Argument(engine, min, argc);
		return false;
	}

	if (argv[0]->type != type1) {
		InkWarn_Wrong_Argument_Type(engine, type1, argv[0]->type);
		return false;
	} else if (argv[1]->type != type2) {
		InkWarn_Wrong_Argument_Type(engine, type2, argv[1]->type);
		return false;
	}

	return true;
}

inline bool checkArgument(bool if_output, Ink_ArgcType argc, Ink_Object **argv,
						  Ink_ArgcType min, Ink_TypeTag type1, Ink_TypeTag type2)
{
	if (argc < min) {
		return false;
	}

	if (argv[0]->type != type1) {
		return false;
	} else if (argv[1]->type != type2) {
		return false;
	}

	return true;
}

inline Ink_Object *addPackage(Ink_InterpreteEngine *engine, Ink_Object *obj, const char *name, Ink_Object *loader)
{
	Ink_Object *pkg;
	obj->setSlot(name, pkg = new Ink_Object(engine));
	pkg->setSlot("load", loader);

	return pkg;
}

inline Ink_Object *addPackage(Ink_InterpreteEngine *engine, Ink_ContextChain *context, const char *name, Ink_Object *loader)
{
	return addPackage(engine, context->context, name, loader);
}

inline void cleanArrayHashTable(Ink_ArrayValue val)
{
	Ink_ArrayValue::size_type i;
	for (i = 0; i < val.size(); i++) {
		delete val[i];
	}
	return;
}

inline Ink_String *getStringVal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *obj)
{
	Ink_Object *tmp;
	if (obj->type == INK_STRING) {
		return as<Ink_String>(obj);
	} else if (obj->type == INK_NUMERIC) {
		std::stringstream ss;
		ss << as<Ink_Numeric>(obj)->value;
		return new Ink_String(engine, std::string(ss.str()));
	} else if ((tmp = getSlotWithProto(engine, context, obj, "to_str"))
			   ->type == INK_FUNCTION) {
		if ((tmp = tmp->call(engine, context))->type != INK_STRING) {
			InkWarn_Invalid_Return_Value_Of_To_String(engine, tmp->type);
			return NULL;
		}
		return as<Ink_String>(tmp);

	}
	return NULL;
}

}

#endif
