#ifndef _NATIVE_GENERAL_H_
#define _NATIVE_GENERAL_H_

#include "../object.h"
#include "../expression.h"
#include "../context.h"
#include "../error.h"

#define NULL_OBJ (new Ink_NullObject())
#define UNDEFINED (new Ink_Undefined())
#define RETURN_FLAG (CGC_interrupt_signal == INTER_RETURN)
#define ASSUME_BASE_TYPE(t) do { \
	if (!assumeType(base, t)) { \
		return NULL_OBJ; \
	} \
} while (0)

inline Ink_Object *getSlotWithProto(Ink_ContextChain *context, Ink_Object *base, const char *name)
{
	return Ink_HashExpression::getSlot(context, base, name);
}

inline Ink_Object *callMethod(Ink_ContextChain *context, Ink_Object *base, const char *method_name,
							  unsigned int argc = 0, Ink_Object **argv = NULL, Ink_Object *this_p = NULL)
{
	if ((base = getSlotWithProto(context, base, method_name))->type != INK_FUNCTION) {
		InkWarn_Failed_Finding_Method(method_name);
		return NULL;
	}
	return base->call(context, argc, argv, this_p);
}

inline bool assumeType(Ink_Object *obj, Ink_TypeTag type_tag)
{
	if (!obj) return false;
	if (obj->type != type_tag) {
		InkWarn_Wrong_Type(type_tag, obj->type);
		return false;
	}

	return true;
}

inline bool checkArgument(unsigned int argc, unsigned int min)
{
	if (argc < min) {
		InkWarn_Too_Less_Argument(min, argc);
		return false;
	}
	return true;
}

inline bool checkArgument(bool if_output, unsigned int argc, Ink_Object **argv,
						  unsigned int min, Ink_TypeTag type1)
{
	if (argc < min) {
		return false;
	}

	if (argv[0]->type != type1) {
		return false;
	}

	return true;
}

inline bool checkArgument(unsigned int argc, Ink_Object **argv,
						  unsigned int min, Ink_TypeTag type1)
{
	if (argc < min) {
		InkWarn_Too_Less_Argument(min, argc);
		return false;
	}

	if (argv[0]->type != type1) {
		InkWarn_Wrong_Argument_Type(type1, argv[0]->type);
		return false;
	}

	return true;
}

inline bool checkArgument(unsigned int argc, Ink_Object **argv,
						  unsigned int min, Ink_TypeTag type1, Ink_TypeTag type2)
{
	if (argc < min) {
		InkWarn_Too_Less_Argument(min, argc);
		return false;
	}

	if (argv[0]->type != type1) {
		InkWarn_Wrong_Argument_Type(type1, argv[0]->type);
		return false;
	} else if (argv[1]->type != type2) {
		InkWarn_Wrong_Argument_Type(type2, argv[1]->type);
		return false;
	}

	return true;
}

inline bool checkArgument(bool if_output, unsigned int argc, Ink_Object **argv,
						  unsigned int min, Ink_TypeTag type1, Ink_TypeTag type2)
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

inline Ink_Object *addPackage(Ink_Object *obj, const char *name, Ink_Object *loader)
{
	Ink_Object *pkg;
	obj->setSlot(name, pkg = new Ink_Object());
	pkg->setSlot("load", loader);

	return pkg;
}

inline Ink_Object *addPackage(Ink_ContextChain *context, const char *name, Ink_Object *loader)
{
	return addPackage(context->context, name, loader);
}

#endif