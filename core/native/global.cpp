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

bool isTrue(Ink_Object *cond)
{
	return cond && cond->isTrue();
	/*
	if (cond->type == INK_NUMERIC) {
		if (as<Ink_Numeric>(cond)->value)
			return true;
		return false;
	}

	return cond->type != INK_NULL && cond->type != INK_UNDEFINED;
	*/
}

Ink_Object *Ink_IfExpression(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *cond;
	Ink_Object *ret;
	Ink_ArgcType i;

	if (!argc) {
		InkWarn_If_Argument_Fault(engine);
		return NULL_OBJ;
	}

	i = 0;
	ret = cond = argv[0];
	if (isTrue(cond)) {
		i++;
		if (i < argc && argv[i]->type == INK_FUNCTION) {
			ret = argv[i]->call(engine, context);
		}
	} else {
		if (i + 1 < argc && argv[i + 1]->type == INK_FUNCTION) {
			i += 2;
		} else {
			i++;
		}
		for (; i < argc; i++) {
			if (argv[i]->type == INK_STRING) {
				if (as<Ink_String>(argv[i])->getValue() == "else") {
					if (++i < argc) {
						if (argv[i]->type == INK_STRING) {
							if (as<Ink_String>(argv[i])->getValue() == "if") {
								if (++i < argc) {
									if (argv[i]->type == INK_ARRAY) {
										if (as<Ink_Array>(argv[i])->value.size() && as<Ink_Array>(argv[i])->value[0]) {
											if (isTrue(as<Ink_Array>(argv[i])->value[0]->getValue())) {
												if (++i < argc) {
													if (argv[i]->type == INK_FUNCTION) {
														ret = argv[i]->call(engine, context);
														break;
													}
												} else {
													InkWarn_If_End_With_Else_If_Has_Condition(engine);
												}
											} else {
												i++;
												continue;
											}
										} else {
											InkWarn_Else_If_Has_No_Condition(engine);
											return ret;
										}
									} else {
										InkWarn_Else_If_Has_No_Condition(engine);
										return ret;
									}
								} else {
									InkWarn_If_End_With_Else_If(engine);
									return ret;
								}
							}
						} else if (argv[i]->type == INK_FUNCTION) {
							ret = argv[i]->call(engine, context);
						}
					} else {
						InkWarn_If_End_With_Else(engine);
						return ret;
					}
				}
			} else if (argv[i]->type == INK_FUNCTION) {
				ret = argv[i]->call(engine, context);
				break;
			}
		}
	}

	return ret;
}

Ink_Object *Ink_WhileExpression(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *cond;
	Ink_Object *block;
	Ink_Object *ret;
	IGC_CollectEngine *gc_engine = engine->getCurrentGC();

	if (argc < 1) {
		InkWarn_While_Argument_Require(engine);
		return NULL_OBJ;
	}

	cond = argv[0];
	block = argc > 1 ? argv[1] : NULL;
	if (cond->type != INK_FUNCTION) {
		InkWarn_Require_Lazy_Expression(engine);
		return NULL_OBJ;
	} else if (block && block->type != INK_FUNCTION) {
		InkWarn_While_Block_Require(engine);
		return NULL_OBJ;
	}

	ret = NULL_OBJ;
	while (isTrue(cond->call(engine, context))) {
		gc_engine->doMark(ret);
		gc_engine->checkGC();
		if (block) {
			ret = block->call(engine, context);
			switch (engine->CGC_interrupt_signal) {
				case INTER_RETURN:
					return engine->CGC_interrupt_value; // fallthrough the signal
				case INTER_DROP:
				case INTER_BREAK:
					return trapSignal(engine); // trap the signal
				case INTER_CONTINUE:
					trapSignal(engine); // trap the signal, but do not return
					continue;
				default: ;
			}
		}
	}

	return ret;
}

Ink_ArrayValue cloneArrayValue(Ink_ArrayValue val)
{
	Ink_ArrayValue ret;
	Ink_ArrayValue::size_type i;
	for (i = 0; i < val.size(); i++) {
		if (val[i])
			ret.push_back(new Ink_HashTable("", val[i]->getValue()));
		else
			ret.push_back(NULL);
	}
	return ret;
}

Ink_Object *Ink_ArrayConstructor(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_ContextChain *local = context->getLocal();
	Ink_Object *ret;

	if (argc) {
		if (argv[0]->type == INK_NUMERIC && argc == 1) {
			ret = new Ink_Array(engine, Ink_ArrayValue(as<Ink_Numeric>(argv[0])->value, NULL));
		} else if (argv[0]->type == INK_ARRAY && argc == 1) {
			ret = new Ink_Array(engine, cloneArrayValue(as<Ink_Array>(argv[0])->value));
		}else {
			Ink_ArrayValue val = Ink_ArrayValue();
			Ink_ArrayValue::size_type i;
			for (i = 0; i < argc; i++) {
				val.push_back(new Ink_HashTable("", argv[i]));
			}
			ret = new Ink_Array(engine, val);
		}
	} else {
		ret = new Ink_Array(engine);
	}

	local->context->setSlot("this", ret);

	return ret;
}

extern Ink_LineNoType current_line_number;
extern const char *yyerror_prefix;
extern pthread_mutex_t ink_parse_lock;

void setParserCurrentLineno(Ink_LineNoType lineno)
{
	pthread_mutex_lock(&ink_parse_lock);
	current_line_number = lineno;
	pthread_mutex_unlock(&ink_parse_lock);
	return;
}

Ink_LineNoType getParserCurrentLineno()
{
	Ink_LineNoType ret;
	pthread_mutex_lock(&ink_parse_lock);
	ret = current_line_number;
	pthread_mutex_unlock(&ink_parse_lock);
	return ret;
}

Ink_Object *Ink_Eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *ret = NULL_OBJ;
	Ink_ExpressionList top_level_backup;
	Ink_LineNoType line_num_backup = getParserCurrentLineno();
	Ink_InterpreteEngine *current_engine = engine;

	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return ret;
	}

	if (current_engine) {
		context->removeLast();

		top_level_backup = current_engine->top_level;

		setParserCurrentLineno(0);
		yyerror_prefix = "from eval: ";
		current_engine->startParse(as<Ink_String>(argv[0])->getValue());
		ret = current_engine->execute(context);

		Ink_insertNativeExpression(current_engine->top_level.begin(),
								   current_engine->top_level.end());
		current_engine->top_level = top_level_backup;

		context->addContext(new Ink_ContextObject(engine));
	} else {
		InkWarn_Eval_Called_Without_Current_Engine(engine);
	}
	setParserCurrentLineno(line_num_backup);

	return ret;
}

bool defined(Ink_Object *obj)
{
	return obj->type != INK_UNDEFINED;
}

Ink_Object *Ink_Print(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 1)) {
		return NULL_OBJ;
	}

	if (argv[0]->type == INK_NUMERIC)
		printf("print(numeric): %f\n", as<Ink_Numeric>(argv[0])->value);
	else if (argv[0]->type == INK_STRING)
		printf("%s\n", as<Ink_String>(argv[0])->getValue().c_str());
	else if (argv[0]->type == INK_NULL)
		printf("(null)\n");
	else if (argv[0]->type == INK_UNDEFINED)
		printf("(undefined)\n");
	else
		printf("print: non-printable type: %d\n", argv[0]->type);

	return NULL_OBJ;
}

Ink_Object *Ink_Import(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_ArgcType i;
	FILE *fp;
	Ink_Object *load, **tmp_argv;
	string *tmp;
	char *current_dir = NULL, *redirect = NULL;
	Ink_InterpreteEngine *current_engine = engine;
	Ink_ExpressionList top_level_backup;
	Ink_LineNoType line_num_backup = getParserCurrentLineno();
	const char *file_path_backup = engine->input_file_path;

	for (i = 0; i < argc; i++) {
		if (argv[i]->type == INK_STRING) {
			tmp = new string(as<Ink_String>(argv[i])->getValue().c_str());
			if (!(fp = fopen(tmp->c_str(), "r"))) {
				InkErr_Failed_Open_File(NULL, tmp->c_str());
				continue;
			}
			engine->input_file_path = tmp->c_str();
			current_dir = getCurrentDir();
			redirect = getBasePath(tmp->c_str());
			if (redirect) {
				changeDir(redirect);
				free(redirect);
			}

			if (current_engine) {
				context->removeLast();
				top_level_backup = current_engine->top_level;
				setParserCurrentLineno(0);

				yyerror_prefix = "from import: ";
				current_engine->startParse(fp);
				current_engine->execute(context);

				engine->CGC_interrupt_signal = INTER_NONE;

				Ink_insertNativeExpression(current_engine->top_level.begin(),
										   current_engine->top_level.end());
				current_engine->top_level = top_level_backup;

				context->addContext(new Ink_ContextObject(engine));
			}
			fclose(fp);

			if (current_dir) {
				changeDir(current_dir);
				free(current_dir);
			}

			delete tmp;
			// run file
		} else {
			// call load method
			if ((load = getSlotWithProto(engine, context, argv[i], "load"))->type == INK_FUNCTION) {
				tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *));
				tmp_argv[0] = argv[i];
				load->call(engine, context, 1, tmp_argv);
				free(tmp_argv);
			} else {
				InkWarn_Not_Package(engine);
			}
		}
	}
	setParserCurrentLineno(line_num_backup);
	engine->input_file_path = file_path_backup;

	return NULL_OBJ;
}

Ink_Object *Ink_TypeName(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (argc < 1) {
		InkWarn_Type_Name_Argument_Require(engine);
		return NULL_OBJ;
	}

	return new Ink_String(engine, engine->getTypeName(argv[0]->type));
}

Ink_Object *Ink_NumVal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Expression *tmp;

	if (!checkArgument(engine, argc, argv, 1, INK_STRING) || as<Ink_String>(argv[0])->getValue() == "") {
		return NULL_OBJ;
	}

	if (!(tmp = Ink_NumericConstant::parse(as<Ink_String>(argv[0])->getValue())))
		return NULL_OBJ;

	Ink_addNativeExpression(tmp);
	return tmp->eval(engine, context);
}

Ink_Object *Ink_BigNum(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *ret = NULL_OBJ;

	if (checkArgument(false, argc, argv, 1, INK_STRING)) {
		ret = new Ink_BigNumeric(engine, as<Ink_String>(argv[0])->getValue());
	} else if (checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		ret = new Ink_BigNumeric(engine, as<Ink_Numeric>(argv[0])->value);
	}

	return ret;
}

Ink_Object *Ink_Debug(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_ArgcType i;
	if (!checkArgument(engine, argc, 1))
		return NULL_OBJ;

	for (i = 0; i < argc; i++) {
		engine->initPrintDebugInfo();
		engine->printDebugInfo(stderr, argv[i]);
	}

	return NULL_OBJ;
}

Ink_Object *Ink_Where(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	engine->printTrace(stderr, engine->getTrace());
	return NULL_OBJ;
}

Ink_Object *Ink_CoroutineCall(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_CoCallList co_call_list = Ink_CoCallList();
	Ink_ArgcType i;
	Ink_CoCallList::size_type j;
	Ink_Object **tmp_argv;
	Ink_Object *ret_val;

	for (i = 0; i < argc; i += 2) {
		tmp_argv = arrayValueToObject(as<Ink_Array>(argv[i + 1])->value);
		co_call_list.push_back(Ink_CoCall(as<Ink_FunctionObject>(argv[i]),
										  as<Ink_Array>(argv[i + 1])->value.size(),
										  tmp_argv));
	}

	ret_val = InkCoCall_call(engine, context, co_call_list);

	for (j = 0; j < co_call_list.size(); j++) {
		free(co_call_list[j].argv);
	}

	return ret_val;
}

void Ink_GlobalMethodInit(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
{
	context->context->setSlot("if", new Ink_FunctionObject(engine, Ink_IfExpression, true));

	Ink_ParamList param_list = Ink_ParamList();
	param_list.push_back(Ink_Parameter(NULL, true));
	Ink_Object *while_func = new Ink_FunctionObject(engine, Ink_WhileExpression, true);
	as<Ink_FunctionObject>(while_func)->param = param_list;
	context->context->setSlot("while", while_func);
	context->context->setSlot("p", new Ink_FunctionObject(engine, Ink_Print));
	context->context->setSlot("eval", new Ink_FunctionObject(engine, Ink_Eval));
	context->context->setSlot("import", new Ink_FunctionObject(engine, Ink_Import));
	context->context->setSlot("typename", new Ink_FunctionObject(engine, Ink_TypeName));
	context->context->setSlot("numval", new Ink_FunctionObject(engine, Ink_NumVal));
	context->context->setSlot("bignum", new Ink_FunctionObject(engine, Ink_BigNum));
	context->context->setSlot("cocall", new Ink_FunctionObject(engine, Ink_CoroutineCall));

	context->context->setSlot("debug", new Ink_FunctionObject(engine, Ink_Debug));
	context->context->setSlot("where", new Ink_FunctionObject(engine, Ink_Where));

	Ink_Object *array_cons = new Ink_FunctionObject(engine, Ink_ArrayConstructor);
	context->context->setSlot("Array", array_cons);

	context->context->setSlot("undefined", UNDEFINED);
	context->context->setSlot("null", NULL_OBJ);
	context->context->setSlot("_", new Ink_Unknown(engine));

	Ink_applyAllModules(engine, context);
}

Ink_Undefined *ink_global_constant_undefined;
Ink_NullObject *ink_global_constant_null;

void Ink_GlobalConstantInit()
{
	ink_global_constant_undefined = new Ink_Undefined(NULL);
	ink_global_constant_null = new Ink_NullObject(NULL);
	return;
}

int numeric_native_method_table_count = 14;
InkNative_MethodTable numeric_native_method_table[] = {
	{"+", new Ink_FunctionObject(NULL, InkNative_Numeric_Add)},
	{"-", new Ink_FunctionObject(NULL, InkNative_Numeric_Sub)},
	{"*", new Ink_FunctionObject(NULL, InkNative_Numeric_Mul)},
	{"/", new Ink_FunctionObject(NULL, InkNative_Numeric_Div)},
	{"%", new Ink_FunctionObject(NULL, InkNative_Numeric_Mod)},
	{"==", new Ink_FunctionObject(NULL, InkNative_Numeric_Equal)},
	{"!=", new Ink_FunctionObject(NULL, InkNative_Numeric_NotEqual)},
	{">", new Ink_FunctionObject(NULL, InkNative_Numeric_Greater)},
	{"<", new Ink_FunctionObject(NULL, InkNative_Numeric_Less)},
	{">=", new Ink_FunctionObject(NULL, InkNative_Numeric_GreaterOrEqual)},
	{"<=", new Ink_FunctionObject(NULL, InkNative_Numeric_LessOrEqual)},
	{"+u", new Ink_FunctionObject(NULL, InkNative_Numeric_Add_Unary)},
	{"-u", new Ink_FunctionObject(NULL, InkNative_Numeric_Sub_Unary)},
	{"to_str", new Ink_FunctionObject(NULL, InkNative_Numeric_ToString)}
};

int string_native_method_table_count = 9;
InkNative_MethodTable string_native_method_table[] = {
	{"+", new Ink_FunctionObject(NULL, InkNative_String_Add)},
	{"<", new Ink_FunctionObject(NULL, InkNative_String_Index)},
	{">", new Ink_FunctionObject(NULL, InkNative_String_Greater)},
	{"<", new Ink_FunctionObject(NULL, InkNative_String_Less)},
	{">=", new Ink_FunctionObject(NULL, InkNative_String_GreaterOrEqual)},
	{"<=", new Ink_FunctionObject(NULL, InkNative_String_LessOrEqual)},
	{"[]", new Ink_FunctionObject(NULL, InkNative_String_Index)},
	{"length", new Ink_FunctionObject(NULL, InkNative_String_Length)},
	{"substr", new Ink_FunctionObject(NULL, InkNative_String_SubStr)},
};

int object_native_method_table_count = 12;
InkNative_MethodTable object_native_method_table[] = {
	{"->", new Ink_FunctionObject(NULL, InkNative_Object_Bond)},
	{"!!", new Ink_FunctionObject(NULL, InkNative_Object_Debond)},
	{"!", new Ink_FunctionObject(NULL, InkNative_Object_Not)},
	{"==", new Ink_FunctionObject(NULL, InkNative_Object_Equal)},
	{"!=", new Ink_FunctionObject(NULL, InkNative_Object_NotEqual)},
	{"[]", new Ink_FunctionObject(NULL, InkNative_Object_Index)},
	{"new", new Ink_FunctionObject(NULL, InkNative_Object_New)},
	{"delete", new Ink_FunctionObject(NULL, InkNative_Object_Delete)},
	{"clone", new Ink_FunctionObject(NULL, InkNative_Object_Clone)},
	{"getter", new Ink_FunctionObject(NULL, InkNative_Object_SetGetter)},
	{"setter", new Ink_FunctionObject(NULL, InkNative_Object_SetSetter)},
	{"each", new Ink_FunctionObject(NULL, InkNative_Object_Each)},
};

int array_native_method_table_count = 6;
InkNative_MethodTable array_native_method_table[] = {
	{"[]", new Ink_FunctionObject(NULL, InkNative_Array_Index)},
	{"push", new Ink_FunctionObject(NULL, InkNative_Array_Push)},
	{"size", new Ink_FunctionObject(NULL, InkNative_Array_Size)},
	{"each", new Ink_FunctionObject(NULL, InkNative_Array_Each)},
	{"remove", new Ink_FunctionObject(NULL, InkNative_Array_Remove)},
	{"rebuild", new Ink_FunctionObject(NULL, InkNative_Array_Rebuild)}
};

inline Ink_ParamList InkNative_Function_GetScope_ParamGenerator()
{
	Ink_ParamList ret = Ink_ParamList();
	ret.push_back(Ink_Parameter(NULL, true));
	return ret;
}

int function_native_method_table_count = 4;
InkNative_MethodTable function_native_method_table[] = {
	{"<<", new Ink_FunctionObject(NULL, InkNative_Function_Insert)},
	{"exp", new Ink_FunctionObject(NULL, InkNative_Function_GetExp)},
	{"[]", new Ink_FunctionObject(NULL, InkNative_Function_RangeCall)},
	{"::", new Ink_FunctionObject(NULL, InkNative_Function_GetScope, InkNative_Function_GetScope_ParamGenerator())}
};

int big_num_native_method_table_count = 15;
InkNative_MethodTable big_num_native_method_table[] = {
	{"+", new Ink_FunctionObject(NULL, InkNative_BigNumeric_Add)},
	{"-", new Ink_FunctionObject(NULL, InkNative_BigNumeric_Sub)},
	{"*", new Ink_FunctionObject(NULL, InkNative_BigNumeric_Mul)},
	{"/", new Ink_FunctionObject(NULL, InkNative_BigNumeric_Div)},
	{"%", new Ink_FunctionObject(NULL, InkNative_BigNumeric_Mod)},
	{"div", new Ink_FunctionObject(NULL, InkNative_BigNumeric_Div)},
	{"==", new Ink_FunctionObject(NULL, InkNative_BigNumeric_Equal)},
	{"!=", new Ink_FunctionObject(NULL, InkNative_BigNumeric_NotEqual)},
	{">", new Ink_FunctionObject(NULL, InkNative_BigNumeric_Greater)},
	{"<", new Ink_FunctionObject(NULL, InkNative_BigNumeric_Less)},
	{">=", new Ink_FunctionObject(NULL, InkNative_BigNumeric_GreaterOrEqual)},
	{"<=", new Ink_FunctionObject(NULL, InkNative_BigNumeric_LessOrEqual)},
	{"+u", new Ink_FunctionObject(NULL, InkNative_BigNumeric_Add_Unary)},
	{"-u", new Ink_FunctionObject(NULL, InkNative_BigNumeric_Sub_Unary)},
	{"to_str", new Ink_FunctionObject(NULL, InkNative_BigNumeric_ToString)}
};