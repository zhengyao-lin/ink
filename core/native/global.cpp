#include <stdio.h>
#include <vector>
#include "native.h"
#include "../object.h"
#include "../context.h"
#include "../expression.h"
#include "../error.h"
#include "../debug.h"
#include "../general.h"
#include "../syntax/syntax.h"
#include "../gc/collect.h"
#include "../interface/engine.h"
#include "../interface/setting.h"
#include "../package/load.h"
#include "../coroutine/coroutine.h"

namespace ink {

bool isTrue(Ink_Object *cond)
{
	return cond && cond->isTrue();
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

static Ink_Object *Ink_ArrayConstructor(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
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

	local->context->setSlot_c("this", ret);

	return ret;
}

static Ink_Object *Ink_Eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *ret = NULL_OBJ;
	Ink_ExpressionList top_level_backup;
	const char *file_name_backup = engine->getFilePath();
	string *new_file_name;
	stringstream strm;

	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return ret;
	}

	InkParser_setParserInfo(1, "from eval: ");

	top_level_backup = engine->top_level;

	strm << file_name_backup << ": eval in line " << engine->current_line_number;
	new_file_name = new string(strm.str());
	engine->setFilePath(new_file_name->c_str());

	context->removeLast();

	engine->startParse(as<Ink_String>(argv[0])->getValue());
	ret = engine->execute(context, false);

	Ink_insertNativeExpression(engine->top_level.begin(),
							   engine->top_level.end());
	engine->top_level = top_level_backup;

	context->addContext(new Ink_ContextObject(engine));

	delete new_file_name;
	engine->setFilePath(file_name_backup);

	return ret;
}

bool defined(Ink_Object *obj)
{
	return obj->type != INK_UNDEFINED;
}

static Ink_Object *Ink_Print(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 1)) {
		return NULL_OBJ;
	}
	string tmp_str;

	if (argv[0]->type == INK_NUMERIC)
		printf("print(numeric): %f\n", as<Ink_Numeric>(argv[0])->value);
	else if (argv[0]->type == INK_STRING) {
		tmp_str = as<Ink_String>(argv[0])->getValue();
		printf("%s\n", tmp_str.c_str());
	}
	else if (argv[0]->type == INK_NULL)
		printf("(null)\n");
	else if (argv[0]->type == INK_UNDEFINED)
		printf("(undefined)\n");
	else
		printf("print: non-printable type: %ld\n", argv[0]->type);

	return NULL_OBJ;
}

static Ink_SizeType import_path_count = 0;
static char **import_path = NULL;

void Ink_addImportPath(const char *path)
{
	import_path = (char **)realloc(import_path, sizeof(char *) * ++import_path_count);
	import_path[import_path_count - 1] = strdup(path);
	return;
}

static Ink_Object *Ink_Import(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_ArgcType i;
	Ink_SizeType j;
	FILE *fp;
	Ink_Object *load, **tmp_argv;
	Ink_ExpressionList top_level_backup;

	char *current_dir = NULL, *redirect = NULL;
	const char *file_name_backup;
	string *tmp;
	string *full_file_name;
	string tmp_path;

	for (i = 0; i < argc; i++) {
		if (argv[i]->type == INK_STRING) {
			/* run a source file */

			tmp = new string(as<Ink_String>(argv[i])->getValue());
			current_dir = getCurrentDir();

			if (!(fp = fopen(tmp->c_str(), "r"))) {
				tmp_path = string(INK_MODULE_DIR) + INK_PATH_SPLIT + tmp->c_str();

				/* cannot found in current dir, search module dir */
				if (!(fp = fopen(tmp_path.c_str(), "r"))) {

					/* cannot found in module dir, search in other import paths */
					for (j = 0; j < import_path_count; j++) {
						tmp_path = string(import_path[j]) + INK_PATH_SPLIT + tmp->c_str();
						if ((fp = fopen(tmp_path.c_str(), "r")) != NULL) {
							break;
						}
					}

					if (!fp) {
						InkError_Failed_Open_File(engine, tmp->c_str());
						free(current_dir);
						delete tmp;
						continue;
					}
				}
				full_file_name = new string(tmp_path);
			} else {
				full_file_name = new string(string(current_dir) + INK_PATH_SPLIT + string(tmp->c_str()));
			}

			/* change dir to the dest dir */
			redirect = getBasePath(full_file_name->c_str());
			if (redirect) {
				changeDir(redirect);
				free(redirect);
			}

			/* backup file name, yacc prefix & lineno and set new */
			file_name_backup = engine->getFilePath();
			engine->setFilePath(full_file_name->c_str());
			InkParser_setParserInfo(1, "from import: ");

			/* remove the last context created for import itself */
			context->removeLast();

			/* backup original top level backup */
			top_level_backup = engine->top_level;

			/* parse and execute */
			engine->startParse(fp);
			engine->execute(context);

			/* store native expressions */
			Ink_insertNativeExpression(engine->top_level.begin(),
									   engine->top_level.end());

			/* restore original top level */
			engine->top_level = top_level_backup;

			/* recreate local context */
			context->addContext(new Ink_ContextObject(engine));

			/* a few clean steps */
			fclose(fp);
			if (current_dir) {
				changeDir(current_dir);
				free(current_dir);
			}
			engine->setFilePath(file_name_backup);

			delete full_file_name;
			delete tmp;
		} else {
			/* call load function of package object */

			if ((load = getSlotWithProto(engine, context, argv[i], "load"))->type == INK_FUNCTION) {
				context->removeLast();
				tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *) * 2);
				tmp_argv[0] = argv[i];
				tmp_argv[1] = context->getLocal()->context;
				load->call(engine, context, 2, tmp_argv);
				free(tmp_argv);
				context->addContext(new Ink_ContextObject(engine));
			} else {
				InkWarn_Not_Package(engine);
			}
		}
	}

	return NULL_OBJ;
}

Ink_Object *InkNative_Import_i(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	return Ink_Import(engine, context, argc, argv, this_p);
}

static Ink_Object *Ink_TypeName(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (argc < 1) {
		InkWarn_Type_Name_Argument_Require(engine);
		return NULL_OBJ;
	}

	return new Ink_String(engine, engine->getTypeName(argv[0]->type));
}

static Ink_Object *Ink_NumVal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
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

static Ink_Object *Ink_Debug(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
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

static Ink_Object *Ink_Where(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	engine->printTrace(stderr, engine->getTrace());
	return NULL_OBJ;
}

static Ink_Object *Ink_CoroutineCall(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_CoCallList co_call_list = Ink_CoCallList();
	Ink_ArgcType i;
	Ink_CoCallList::size_type j;
	Ink_Object **tmp_argv;
	Ink_Object *ret_val;

	for (i = 0; i < argc; i += 2) {
		if (argv[i]->type != INK_FUNCTION || argv[i + 1]->type != INK_ARRAY) {
			InkWarn_Cocall_Argument_Require(engine);
			ret_val = NULL_OBJ;
			goto END;
		}
		tmp_argv = arrayValueToObjects(as<Ink_Array>(argv[i + 1])->value);
		co_call_list.push_back(Ink_CoCall(as<Ink_FunctionObject>(argv[i]),
										  as<Ink_Array>(argv[i + 1])->value.size(),
										  tmp_argv));
	}

	ret_val = InkCoCall_call(engine, context, co_call_list);

END:
	for (j = 0; j < co_call_list.size(); j++) {
		free(co_call_list[j].argv);
	}

	return ret_val;
}

static Ink_Object *Ink_Auto_Missing(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	string tmp_str;

	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return UNDEFINED;
	}

	tmp_str = as<Ink_String>(argv[0])->getValue();

	return Ink_IdentifierExpression::getContextSlot(engine, context, tmp_str.c_str(),
													Ink_EvalFlag(), false);
}

Ink_Object *InkNative_Auto_Missing_i(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	return Ink_Auto_Missing(engine, context, argc, argv, this_p);
}

static Ink_Object *Ink_Abort(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_disposeEnv();
	abort();
	return NULL_OBJ;
}

static Ink_Object *Ink_RegisterSignal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	engine->addCustomInterruptSignal(as<Ink_String>(argv[0])->getValue());

	return NULL_OBJ;
}

static Ink_Object *Ink_DeleteSignal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, engine->deleteCustomInterruptSignal(as<Ink_String>(argv[0])->getValue()));
}

void Ink_GlobalMethodInit(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
{
	context->context->setSlot_c("p", new Ink_FunctionObject(engine, Ink_Print));
	context->context->setSlot_c("eval", new Ink_FunctionObject(engine, Ink_Eval));
	context->context->setSlot_c("import", new Ink_FunctionObject(engine, Ink_Import));
	context->context->setSlot_c("typename", new Ink_FunctionObject(engine, Ink_TypeName));
	context->context->setSlot_c("numval", new Ink_FunctionObject(engine, Ink_NumVal));
	context->context->setSlot_c("cocall", new Ink_FunctionObject(engine, Ink_CoroutineCall));

	context->context->setSlot_c("abort", new Ink_FunctionObject(engine, Ink_Abort));
	context->context->setSlot_c("regsig", new Ink_FunctionObject(engine, Ink_RegisterSignal));
	context->context->setSlot_c("delsig", new Ink_FunctionObject(engine, Ink_DeleteSignal));

	context->context->setSlot_c("debug", new Ink_FunctionObject(engine, Ink_Debug));
	context->context->setSlot_c("where", new Ink_FunctionObject(engine, Ink_Where));

	Ink_Object *array_cons = new Ink_FunctionObject(engine, Ink_ArrayConstructor);
	context->context->setSlot_c("Array", array_cons);

	context->context->setSlot_c("undefined", UNDEFINED);
	context->context->setSlot_c("null", NULL_OBJ);
	context->context->setSlot_c("_", new Ink_Unknown(engine));

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

int numeric_native_method_table_count = 17;
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
	{"to_str", new Ink_FunctionObject(NULL, InkNative_Numeric_ToString)},
	{"ceil", new Ink_FunctionObject(NULL, InkNative_Numeric_Ceil)},
	{"floor", new Ink_FunctionObject(NULL, InkNative_Numeric_Floor)},
	{"round", new Ink_FunctionObject(NULL, InkNative_Numeric_Round)}
};

int string_native_method_table_count = 10;
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
	{"to_str", new Ink_FunctionObject(NULL, InkNative_String_ToString)},
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
	{"each", new Ink_FunctionObject(NULL, InkNative_Object_Each, true)}
};

int array_native_method_table_count = 7;
InkNative_MethodTable array_native_method_table[] = {
	{"[]", new Ink_FunctionObject(NULL, InkNative_Array_Index)},
	{"push", new Ink_FunctionObject(NULL, InkNative_Array_Push)},
	{"size", new Ink_FunctionObject(NULL, InkNative_Array_Size)},
	{"each", new Ink_FunctionObject(NULL, InkNative_Array_Each, true)},
	{"last", new Ink_FunctionObject(NULL, InkNative_Array_Last)},
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

int explist_native_method_table_count = 2;
InkNative_MethodTable explist_native_method_table[] = {
	{"to_array", new Ink_FunctionObject(NULL, InkNative_ExpList_ToArray)},
	{"<<", new Ink_FunctionObject(NULL, InkNative_ExpList_Insert)}
};

}
