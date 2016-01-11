#include <stdio.h>
#include <vector>
#include "../object.h"
#include "../context.h"
#include "../expression.h"
#include "../error.h"
#include "../debug.h"
#include "../general.h"
#include "../interface/engine.h"
#include "../interface/setting.h"
#include "../package/load.h"
#include "../coroutine/coroutine.h"
#include "native.h"

extern Ink_ExpressionList native_exp_list;
extern InterruptSignal CGC_interrupt_signal;

bool isTrue(Ink_Object *cond)
{
	if (cond->type == INK_NUMERIC) {
		if (as<Ink_Numeric>(cond)->value)
			return true;
		return false;
	}

	return cond->type != INK_NULL && cond->type != INK_UNDEFINED;
}

Ink_Object *Ink_IfExpression(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *cond;
	Ink_Object *ret;
	unsigned int i;

	if (!argc) {
		InkWarn_If_Argument_Fault();
		return new Ink_NullObject();
	}

	i = 0;
	ret = cond = argv[0];
	if (isTrue(cond)) {
		i++;
		if (i < argc && argv[i]->type == INK_FUNCTION) {
			ret = argv[i]->call(context);
		}
	} else {
		if (i + 1 < argc && argv[i + 1]->type == INK_FUNCTION) {
			i += 2;
		} else {
			i++;
		}
		for (; i < argc; i++) {
			if (argv[i]->type == INK_STRING) {
				if (as<Ink_String>(argv[i])->value == "else") {
					if (++i < argc) {
						if (argv[i]->type == INK_STRING) {
							if (as<Ink_String>(argv[i])->value == "if") {
								if (++i < argc) {
									if (argv[i]->type == INK_ARRAY) {
										if (as<Ink_Array>(argv[i])->value.size() && as<Ink_Array>(argv[i])->value[0]) {
											if (isTrue(as<Ink_Array>(argv[i])->value[0]->getValue())) {
												if (++i < argc) {
													if (argv[i]->type == INK_FUNCTION) {
														ret = argv[i]->call(context);
														break;
													}
												} else {
													InkWarn_If_End_With_Else_If_Has_Condition();
												}
											} else {
												i++;
												continue;
											}
										} else {
											InkWarn_Else_If_Has_No_Condition();
											return ret;
										}
									} else {
										InkWarn_Else_If_Has_No_Condition();
										return ret;
									}
								} else {
									InkWarn_If_End_With_Else_If();
									return ret;
								}
							}
						} else if (argv[i]->type == INK_FUNCTION) {
							ret = argv[i]->call(context);
						}
					} else {
						InkWarn_If_End_With_Else();
						return ret;
					}
				}
			} else if (argv[i]->type == INK_FUNCTION) {
				ret = argv[i]->call(context);
				break;
			}
		}
	}

	return ret;
}

Ink_Object *Ink_WhileExpression(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *cond;
	Ink_Object *block;
	Ink_Object *ret;

	if (argc < 2) {
		InkWarn_While_Argument_Require();
		return new Ink_NullObject();
	}

	cond = argv[0];
	block = argv[1];
	if (cond->type != INK_FUNCTION) {
		InkWarn_Require_Lazy_Expression();
		return new Ink_NullObject();
	} else if (block->type != INK_FUNCTION) {
		InkWarn_While_Block_Require();
		return new Ink_NullObject();
	}

	ret = new Ink_NullObject();
	while (isTrue(cond->call(context))) {
		ret = block->call(context);
		switch (CGC_interrupt_signal) {
			case INTER_RETURN:
				return CGC_interrupt_value; // fallthrough the signal
			case INTER_DROP:
			case INTER_BREAK:
				return trapSignal(); // trap the signal
			case INTER_CONTINUE:
				trapSignal(); // trap the signal, but do not return
				continue;
			default: ;
		}
	}

	return ret;
}

Ink_ArrayValue cloneArrayValue(Ink_ArrayValue val)
{
	Ink_ArrayValue ret;
	unsigned int i;
	for (i = 0; i < val.size(); i++) {
		if (val[i])
			ret.push_back(new Ink_HashTable("", val[i]->getValue()));
		else
			ret.push_back(NULL);
	}
	return ret;
}

Ink_Object *Ink_ArrayConstructor(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_ContextChain *local = context->getLocal();
	Ink_Object *ret;

	if (argc) {
		if (argv[0]->type == INK_NUMERIC && argc == 1) {
			ret = new Ink_Array(Ink_ArrayValue(as<Ink_Numeric>(argv[0])->value, NULL));
		} else if (argv[0]->type == INK_ARRAY && argc == 1) {
			ret = new Ink_Array(cloneArrayValue(as<Ink_Array>(argv[0])->value));
		}else {
			Ink_ArrayValue val = Ink_ArrayValue();
			unsigned int i;
			for (i = 0; i < argc; i++) {
				val.push_back(new Ink_HashTable("", argv[i]));
			}
			ret = new Ink_Array(val);
		}
	} else {
		ret = new Ink_Array();
	}

	local->context->setSlot("this", ret);

	return ret;
}

extern int current_line_number;
extern int inkerr_current_line_number;
extern const char *yyerror_prefix;

Ink_Object *Ink_Eval(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *ret = NULL_OBJ;
	Ink_ExpressionList top_level_backup;
	int line_num_backup = current_line_number;
	Ink_InterpreteEngine *current_engine = Ink_getCurrentEngine();

	if (!checkArgument(argc, argv, 1, INK_STRING)) {
		return ret;
	}

	if (current_engine) {
		context->removeLast();

		top_level_backup = current_engine->top_level;

		current_line_number = inkerr_current_line_number;
		yyerror_prefix = "from eval: ";
		current_engine->startParse(as<Ink_String>(argv[0])->value);
		ret = current_engine->execute(context);

		native_exp_list.insert(native_exp_list.end(),
							   current_engine->top_level.begin(),
							   current_engine->top_level.end());
		current_engine->top_level = top_level_backup;

		context->addContext(new Ink_ContextObject());
	} else {
		InkWarn_Eval_Called_Without_Current_Engine();
	}
	current_line_number = line_num_backup;

	return ret;
}

bool defined(Ink_Object *obj)
{
	return obj->type != INK_UNDEFINED;
}

Ink_Object *Ink_Print(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(argc, 1)) {
		return NULL_OBJ;
	}

	if (argv[0]->type == INK_NUMERIC)
		printf("print(numeric): %f\n", as<Ink_Numeric>(argv[0])->value);
	else if (argv[0]->type == INK_STRING)
		printf("%s\n", as<Ink_String>(argv[0])->value.c_str());
	else if (argv[0]->type == INK_NULL)
		printf("(null)\n");
	else if (argv[0]->type == INK_UNDEFINED)
		printf("(undefined)\n");
	else
		printf("print: non-printable type: %d\n", argv[0]->type);

	return NULL_OBJ;
}

Ink_Object *Ink_Import(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	unsigned int i;
	FILE *fp;
	Ink_Object *load, **tmp_argv;
	const char *tmp;
	char *current_dir = NULL, *redirect = NULL;
	Ink_InterpreteEngine *current_engine = Ink_getCurrentEngine();
	Ink_ExpressionList top_level_backup;
	int line_num_backup = current_line_number;

	for (i = 0; i < argc; i++) {
		if (argv[i]->type == INK_STRING) {
			tmp = as<Ink_String>(argv[i])->value.c_str();
			if (!(fp = fopen(tmp, "r"))) {
				InkErr_Failed_Open_File(tmp);
				continue;
			}
			current_dir = getCurrentDir();
			redirect = getBasePath(tmp);
			if (redirect) {
				changeDir(redirect);
				free(redirect);
			}

			if (current_engine) {
				context->removeLast();
				top_level_backup = current_engine->top_level;
				current_line_number = inkerr_current_line_number;

				yyerror_prefix = "from import: ";
				current_engine->startParse(fp);
				current_engine->execute(context);

				CGC_interrupt_signal = INTER_NONE;

				native_exp_list.insert(native_exp_list.end(),
									   current_engine->top_level.begin(),
									   current_engine->top_level.end());
				current_engine->top_level = top_level_backup;

				context->addContext(new Ink_ContextObject());
			}
			fclose(fp);

			if (current_dir) {
				changeDir(current_dir);
				free(current_dir);
			}
			// run file
		} else {
			// call load method
			if ((load = getSlotWithProto(context, argv[i], "load"))->type == INK_FUNCTION) {
				tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *));
				tmp_argv[0] = argv[i];
				load->call(context, 1, tmp_argv);
				free(tmp_argv);
			} else {
				InkWarn_Not_Package();
			}
		}
	}
	current_line_number = line_num_backup;

	return NULL_OBJ;
}

Ink_Object *Ink_TypeName(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (argc < 1) {
		InkWarn_Type_Name_Argument_Require();
		return NULL_OBJ;
	}

	return new Ink_String(getTypeName(argv[0]->type));
}

Ink_Object *Ink_NumVal(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Expression *tmp;

	if (!checkArgument(argc, argv, 1, INK_STRING) || as<Ink_String>(argv[0])->value == "") {
		return NULL_OBJ;
	}

	if (!(tmp = Ink_NumericConstant::parse(as<Ink_String>(argv[0])->value)))
		return NULL_OBJ;

	native_exp_list.push_back(tmp);
	return tmp->eval(context);
}

Ink_Object *Ink_Debug(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	unsigned int i;
	if (!checkArgument(argc, 1))
		return NULL_OBJ;

	for (i = 0; i < argc; i++) {
		DBG_initPrintDebugInfo();
		DBG_printDebugInfo(stderr, argv[i]);
	}

	return NULL_OBJ;
}

Ink_Object *Ink_Where(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	DBG_printTrace(stderr, Ink_getCurrentEngine()->getTrace());
	return NULL_OBJ;
}

Ink_Object *Ink_CallSync(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_SyncCallList sync_call_list = Ink_SyncCallList();
	unsigned int i;
	Ink_Object **tmp_argv;
	Ink_Object *ret_val;

	for (i = 0; i < argc; i += 2) {
		tmp_argv = arrayValueToObject(as<Ink_Array>(argv[i + 1])->value);
		sync_call_list.push_back(Ink_SyncCall(as<Ink_FunctionObject>(argv[i]), as<Ink_Array>(argv[i + 1])->value.size(),
											  tmp_argv));
	}

	ret_val = Ink_callSync(context, sync_call_list);

	for (i = 0; i < sync_call_list.size(); i++) {
		free(sync_call_list[i].argv);
	}

	return ret_val;
}

Ink_Object *InkNative_Object_New(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p);
void Ink_GlobalMethodInit(Ink_ContextChain *context)
{
	context->context->setSlot("if", new Ink_FunctionObject(Ink_IfExpression, true));

	Ink_ParamList param_list = Ink_ParamList();
	param_list.push_back(Ink_Parameter(NULL, true));
	Ink_Object *while_func = new Ink_FunctionObject(Ink_WhileExpression, true);
	as<Ink_FunctionObject>(while_func)->param = param_list;
	context->context->setSlot("while", while_func);
	context->context->setSlot("p", new Ink_FunctionObject(Ink_Print));
	context->context->setSlot("eval", new Ink_FunctionObject(Ink_Eval));
	context->context->setSlot("import", new Ink_FunctionObject(Ink_Import));
	context->context->setSlot("typename", new Ink_FunctionObject(Ink_TypeName));
	context->context->setSlot("numval", new Ink_FunctionObject(Ink_NumVal));
	context->context->setSlot("call_sync", new Ink_FunctionObject(Ink_CallSync));

	context->context->setSlot("debug", new Ink_FunctionObject(Ink_Debug));
	context->context->setSlot("where", new Ink_FunctionObject(Ink_Where));

	Ink_Object *array_cons = new Ink_FunctionObject(Ink_ArrayConstructor);
	context->context->setSlot("Array", array_cons);
	array_cons->setSlot("new", new Ink_FunctionObject(InkNative_Object_New));

	context->context->setSlot("undefined", new Ink_Undefined());
	context->context->setSlot("null", new Ink_NullObject());
	context->context->setSlot("_", new Ink_Unknown());

	loadAllModules(context);
}

int numeric_native_method_table_count = 14;
InkNative_MethodTable numeric_native_method_table[] = {
	{"+", new Ink_FunctionObject(InkNative_Numeric_Add)},
	{"-", new Ink_FunctionObject(InkNative_Numeric_Sub)},
	{"*", new Ink_FunctionObject(InkNative_Numeric_Mul)},
	{"/", new Ink_FunctionObject(InkNative_Numeric_Div)},
	{"%", new Ink_FunctionObject(InkNative_Numeric_Mod)},
	{"==", new Ink_FunctionObject(InkNative_Numeric_Equal)},
	{"!=", new Ink_FunctionObject(InkNative_Numeric_NotEqual)},
	{">", new Ink_FunctionObject(InkNative_Numeric_Greater)},
	{"<", new Ink_FunctionObject(InkNative_Numeric_Less)},
	{">=", new Ink_FunctionObject(InkNative_Numeric_GreaterOrEqual)},
	{"<=", new Ink_FunctionObject(InkNative_Numeric_LessOrEqual)},
	{"+u", new Ink_FunctionObject(InkNative_Numeric_Add_Unary)},
	{"-u", new Ink_FunctionObject(InkNative_Numeric_Sub_Unary)},
	{"to_str", new Ink_FunctionObject(InkNative_Numeric_ToString)}
};

int string_native_method_table_count = 8;
InkNative_MethodTable string_native_method_table[] = {
	{"+", new Ink_FunctionObject(InkNative_String_Add)},
	{"<", new Ink_FunctionObject(InkNative_String_Index)},
	{">", new Ink_FunctionObject(InkNative_String_Greater)},
	{"<", new Ink_FunctionObject(InkNative_String_Less)},
	{">=", new Ink_FunctionObject(InkNative_String_GreaterOrEqual)},
	{"<=", new Ink_FunctionObject(InkNative_String_LessOrEqual)},
	{"[]", new Ink_FunctionObject(InkNative_String_Index)},
	{"length", new Ink_FunctionObject(InkNative_String_Length)}
};

int object_native_method_table_count = 12;
InkNative_MethodTable object_native_method_table[] = {
	{"->", new Ink_FunctionObject(InkNative_Object_Bond)},
	{"!!", new Ink_FunctionObject(InkNative_Object_Debond)},
	{"!", new Ink_FunctionObject(InkNative_Object_Not)},
	{"==", new Ink_FunctionObject(InkNative_Object_Equal)},
	{"!=", new Ink_FunctionObject(InkNative_Object_NotEqual)},
	{"[]", new Ink_FunctionObject(InkNative_Object_Index)},
	{"new", new Ink_FunctionObject(InkNative_Object_New)},
	{"delete", new Ink_FunctionObject(InkNative_Object_Delete)},
	{"clone", new Ink_FunctionObject(InkNative_Object_Clone)},
	{"getter", new Ink_FunctionObject(InkNative_Object_SetGetter)},
	{"setter", new Ink_FunctionObject(InkNative_Object_SetSetter)},
	{"each", new Ink_FunctionObject(InkNative_Object_Each)},
};

int array_native_method_table_count = 6;
InkNative_MethodTable array_native_method_table[] = {
	{"[]", new Ink_FunctionObject(InkNative_Array_Index)},
	{"push", new Ink_FunctionObject(InkNative_Array_Push)},
	{"size", new Ink_FunctionObject(InkNative_Array_Size)},
	{"each", new Ink_FunctionObject(InkNative_Array_Each)},
	{"remove", new Ink_FunctionObject(InkNative_Array_Remove)},
	{"rebuild", new Ink_FunctionObject(InkNative_Array_Rebuild)}
};

inline Ink_ParamList InkNative_Function_GetScope_ParamGenerator()
{
	Ink_ParamList ret = Ink_ParamList();
	ret.push_back(Ink_Parameter(NULL, true));
	return ret;
}

int function_native_method_table_count = 4;
InkNative_MethodTable function_native_method_table[] = {
	{"<<", new Ink_FunctionObject(InkNative_Function_Insert)},
	{"exp", new Ink_FunctionObject(InkNative_Function_GetExp)},
	{"[]", new Ink_FunctionObject(InkNative_Function_RangeCall)},
	{"::", new Ink_FunctionObject(InkNative_Function_GetScope, InkNative_Function_GetScope_ParamGenerator())}
};