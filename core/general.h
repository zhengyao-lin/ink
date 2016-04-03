#ifndef _CORE_GENERAL_H_
#define _CORE_GENERAL_H_

#include <vector>
#include <string>
#include <stdlib.h>
#include "inttype.h"
#include "numeric.h"
#include "../includes/universal.h"

#define UNDEFINED (new Ink_Undefined(engine))
#define NULL_OBJ (new Ink_NullObject(engine))
#define TRUE_OBJ (new Ink_Numeric(engine, 1))
#define FALSE_OBJ (new Ink_Numeric(engine, 0))

#define RETURN_FLAG (engine->CGC_interrupt_signal == INTER_RETURN)
#define BREAK_FLAG (engine->CGC_interrupt_signal == INTER_BREAK)
#define CONTINUE_FLAG (engine->CGC_interrupt_signal == INTER_CONTINUE)
#define DROP_FLAG (engine->CGC_interrupt_signal == INTER_DROP)

#define DEFAULT_SIGNAL (INTER_RETURN | INTER_BREAK | INTER_CONTINUE | INTER_DROP)
#define ALL_SIGNAL (INTER_RETURN | INTER_BREAK | \
					INTER_CONTINUE | INTER_DROP | \
					INTER_THROW | INTER_RETRY | INTER_EXIT)

namespace ink {

inline int removeDir(const std::string path, bool if_delete_sub = true);
inline char *getCurrentDir();
inline int changeDir(const char *path);

}

#if defined(INK_PLATFORM_LINUX)
#include <unistd.h>

namespace ink {

inline int removeDir(const std::string path, bool if_delete_sub)
{
	return system(("rm -r \"" + path + "\"").c_str());
}

inline char *getCurrentDir()
{
	return getcwd(NULL, 0);
}

inline int changeDir(const char *path)
{
	return chdir(path);
}

}
#elif defined(INK_PLATFORM_WIN32)
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <direct.h>

namespace ink {

inline int removeDir(const std::string path, bool if_delete_sub)
{
	bool has_sub_dir = false;
	HANDLE file_handle;
	std::string tmp_file_path;
	std::string pattern;
	WIN32_FIND_DATA file_info;

	pattern = path + "\\*";
	file_handle = FindFirstFile(pattern.c_str(), &file_info);
	if (file_handle != INVALID_HANDLE_VALUE) {
		do {
			if (file_info.cFileName[0] != '.') {
				tmp_file_path.erase();
				tmp_file_path = path + "\\" + file_info.cFileName;

				if (file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (if_delete_sub) {
						int i = removeDir(tmp_file_path, if_delete_sub);
						if (i) return i;
					} else has_sub_dir = true;
				} else {
				  if (SetFileAttributes(tmp_file_path.c_str(),
				                         FILE_ATTRIBUTE_NORMAL) == false)
				    return GetLastError();

				  if (DeleteFile(tmp_file_path.c_str()) == false)
				    return GetLastError();
				}
			}
		} while (FindNextFile(file_handle, &file_info) == true);

		FindClose(file_handle);

		DWORD dwError = GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
			return dwError;
		else {
			if (!has_sub_dir) {
				if (SetFileAttributes(path.c_str(),
									   FILE_ATTRIBUTE_NORMAL) == false)
				return GetLastError();

				if (RemoveDirectory(path.c_str()) == false)
					return GetLastError();
			}
		}
	}

	return 0;
}

inline char *getCurrentDir()
{
	return _getcwd(NULL, 0);
}

inline int changeDir(const char *path)
{
	return _chdir(path);
}

}
#endif

namespace ink {

typedef Ink_UInt64 Ink_InterruptSignal;
#define INTER_NONE			((Ink_InterruptSignal)INTER_NONE_tag)
#define INTER_RETURN		((Ink_InterruptSignal)INTER_RETURN_tag)
#define INTER_BREAK			((Ink_InterruptSignal)INTER_BREAK_tag)
#define INTER_CONTINUE		((Ink_InterruptSignal)INTER_CONTINUE_tag)
#define INTER_DROP			((Ink_InterruptSignal)INTER_DROP_tag)
#define INTER_THROW			((Ink_InterruptSignal)INTER_THROW_tag)
#define INTER_RETRY			((Ink_InterruptSignal)INTER_RETRY_tag)
#define INTER_EXIT			((Ink_InterruptSignal)INTER_EXIT_tag)
#define INTER_LAST			((Ink_InterruptSignal)INTER_LAST_tag)

typedef enum {
	INTER_NONE_tag 		= 1 << 1,
	INTER_RETURN_tag 	= 1 << 2,
	INTER_BREAK_tag 	= 1 << 3,
	INTER_CONTINUE_tag 	= 1 << 4,
	INTER_DROP_tag 		= 1 << 5,
	INTER_THROW_tag 	= 1 << 6,
	INTER_RETRY_tag 	= 1 << 7,
	INTER_EXIT_tag 		= 1 << 8,
	INTER_LAST_tag 		= 1 << 16
} Ink_InterruptSignal_tag;

class Ink_Expression;
class Ink_Argument;
class Ink_Object;
class Ink_InterpreteEngine;
class Ink_HashTable;
class Ink_Undefined;
class Ink_NullObject;

typedef Ink_InterruptSignal Ink_InterruptSignalTrap;
typedef std::vector<Ink_Expression *> Ink_ExpressionList;
typedef std::vector<Ink_Argument *> Ink_ArgumentList;

typedef Ink_UInt32 Ink_ArgcType;
typedef Ink_SInt32 IGC_MarkType;
typedef Ink_UInt64 IGC_ObjectCountType;
typedef Ink_SInt64 Ink_LineNoType;
// typedef double Ink_NumericValue;
typedef std::vector<Ink_HashTable *> Ink_ArrayValue;

inline Ink_SInt64 getInt(Ink_NumericValue val)
{
	return val.toInt();
}

inline double getFloat(Ink_NumericValue val)
{
	return val.toFloat();
}

inline bool getBool(Ink_NumericValue val)
{
	return val.toBool();
}

Ink_SInt64 getInt(Ink_Object *obj);
double getFloat(Ink_Object *obj);
bool getBool(Ink_Object *val);

typedef Ink_UInt64 Ink_ModuleID;
#define INK_CORE_MOD_ID (0)

typedef Ink_UInt64 Ink_ExceptionCode;
enum Ink_ExceptionCode_tag {
	INK_EXCODE_UNDEFINED = 0,
	INK_EXCODE_CUSTOM_START = 1,
	INK_EXCODE_ERROR_CALLING_NON_FUNCTION_OBJECT = 1,
	INK_EXCODE_ERROR_CALLING_UNDEFINED_OBJECT,
	INK_EXCODE_ERROR_YIELD_WITHOUT_COROUTINE,
	INK_EXCODE_ERROR_FAILED_OPEN_FILE,

	INK_EXCODE_WARN_ASSIGNING_UNASSIGNABLE_EXPRESSION,
	INK_EXCODE_WARN_HASH_NOT_FOUND,
	INK_EXCODE_WARN_GET_SLOT_OF_UNDEFINED,
	INK_EXCODE_WARN_INSERT_NON_FUNCTION_OBJECT,
	INK_EXCODE_WARN_ARRAY_INDEX_EXCEED,
	INK_EXCODE_WARN_BONDING_FAILED,
	INK_EXCODE_WARN_SELF_BONDING,
	INK_EXCODE_WARN_GET_NON_ARRAY_INDEX,
	INK_EXCODE_WARN_FUNCTION_RANGE_CALL_ARGUMENT_ERROR,
	INK_EXCODE_WARN_FUNCTION_NON_RANGE_CALL,
	INK_EXCODE_WARN_INCORRECT_RANGE_TYPE,
	INK_EXCODE_WARN_GET_NON_FUNCTION_EXP,
	INK_EXCODE_WARN_REBUILD_NON_ARRAY,
	INK_EXCODE_WARN_INVALID_ELEMENT_FOR_REBUILD,
	INK_EXCODE_WARN_REMOVE_ARGUMENT_REQUIRE,
	INK_EXCODE_WARN_TOO_HUGE_INDEX,
	INK_EXCODE_WARN_EACH_ARGUMENT_REQUIRE,
	INK_EXCODE_WARN_FAILED_FINDING_METHOD,
	INK_EXCODE_WARN_WRONG_TYPE,
	INK_EXCODE_WARN_WRONG_BASE_TYPE,
	INK_EXCODE_WARN_TOO_LESS_ARGUMENT,
	INK_EXCODE_WARN_WRONG_ARGUMENT_TYPE,
	INK_EXCODE_WARN_NOT_PACKAGE,
	INK_EXCODE_WARN_SETTING_UNASSIGNABLE_GETTER,
	INK_EXCODE_WARN_SETTING_UNASSIGNABLE_SETTER,
	INK_EXCODE_WARN_TYPE_NAME_ARGUMENT_REQUIRE,
	INK_EXCODE_WARN_WITH_ATTACHMENT_REQUIRE,
	INK_EXCODE_WARN_FAILED_OPEN_FILE,
	INK_EXCODE_WARN_FAILED_FIND_MOD,
	INK_EXCODE_WARN_FAILED_LOAD_MOD,
	INK_EXCODE_WARN_FAILED_FIND_LOADER,
	INK_EXCODE_WARN_FAILED_FIND_INIT,
	INK_EXCODE_WARN_FAILED_INIT_MOD,
	INK_EXCODE_WARN_HASH_TABLE_MAPPING_EXPECT_STRING,
	INK_EXCODE_WARN_DELETE_FUNCTION_ARGUMENT_REQUIRE,
	INK_EXCODE_WARN_EVAL_CALLED_WITHOUT_CURRENT_ENGINE,
	INK_EXCODE_WARN_INVALID_ARGUMENT_FOR_STRING_ADD,
	INK_EXCODE_WARN_INVALID_RETURN_VALUE_OF_TO_STRING,
	INK_EXCODE_WARN_LOAD_MOD_ON_WRONG_OS,
	INK_EXCODE_WARN_NO_FILE_IN_MOD,
	INK_EXCODE_WARN_ACTOR_CONFLICT,
	INK_EXCODE_WARN_FAILED_CREATE_PROCESS,
	INK_EXCODE_WARN_CIRCULAR_PROTOTYPE_REFERENCE,
	INK_EXCODE_WARN_STRING_INDEX_EXCEED,
	INK_EXCODE_WARN_SUB_STRING_EXCEED,
	INK_EXCODE_WARN_UNREACHABLE_BONDING,
	INK_EXCODE_WARN_DIVIDED_BY_ZERO,
	INK_EXCODE_WARN_UNDEFINED_CUSTOM_INTERRUPT_NAME,
	INK_EXCODE_WARN_UNREGISTERED_INTERRUPT_SIGNAL,
	INK_EXCODE_WARN_TRAPPING_UNTRAPPED_SIGNAL,
	INK_EXCODE_WARN_PACKAGE_BROKEN,
	INK_EXCODE_WARN_FAILED_CREATE_COROUTINE,
	INK_EXCODE_WARN_COCALL_ARGUMENT_REQUIRE,
	INK_EXCODE_WARN_WRONG_RET_VAL_FOR_COMPARE,
	INK_EXCODE_WARN_ASSIGN_FIXED,
	INK_EXCODE_WARN_FAILED_GET_CONSTANT,
	INK_EXCODE_WARN_FIX_REQUIRE_ASSIGNABLE_ARGUMENT,
	INK_EXCODE_WRAN_SLICE_REQUIRE_NUMERIC,
	INK_EXCODE_WRAN_SLICE_REQUIRE_NON_ZERO_RANGE,
	INK_EXCODE_LAST
};

typedef void (*IGC_Marker)(Ink_InterpreteEngine *engine, Ink_Object *obj);
typedef std::pair<Ink_HashTable *, Ink_HashTable *> IGC_Bonding;
typedef std::vector<IGC_Bonding> IGC_BondingList;

template <typename T1, typename T2, typename T3>
class triple {
public:
	T1 first;
	T2 second;
	T3 third;

	triple(T1 first)
	: first(first)
	{ }

	triple(T1 first, T2 second)
	: first(first), second(second)
	{ }

	triple(T1 first, T2 second, T3 third)
	: first(first), second(second), third(third)
	{ }
};

class Ink_Argument {
public:
	Ink_Expression *arg;

	bool is_expand;
	Ink_Expression *expandee;

	Ink_Argument(Ink_Expression *arg)
	: arg(arg), is_expand(false), expandee(NULL)
	{ }

	Ink_Argument(bool is_expand, Ink_Expression *expandee)
	: arg(NULL), is_expand(is_expand), expandee(expandee)
	{ }

	~Ink_Argument();
};

class Ink_Parameter {
public:
	std::string *name;
	bool is_ref;
	bool is_variant;
	bool is_optional;

	Ink_Parameter(std::string *name)
	: name(name), is_ref(false), is_variant(false), is_optional(true)
	{ }

	Ink_Parameter(std::string *name, bool is_ref)
	: name(name), is_ref(is_ref), is_variant(false), is_optional(true)
	{ }

	Ink_Parameter(std::string *name, bool is_ref, bool is_variant)
	: name(name), is_ref(is_ref), is_variant(is_variant), is_optional(true)
	{ }

	Ink_Parameter(std::string *name, bool is_ref, bool is_variant, bool is_optional)
	: name(name), is_ref(is_ref), is_variant(is_variant), is_optional(is_optional)
	{ }
};
typedef std::vector<Ink_Parameter> Ink_ParamList;

inline bool hasSignal(Ink_InterruptSignalTrap set, Ink_InterruptSignal sig)
{
	return (~(~set | sig) != set) && sig < INTER_LAST;
}

inline Ink_InterruptSignalTrap addSignal(Ink_InterruptSignalTrap set, Ink_InterruptSignal sign)
{
	return set | sign;
}

class Ink_FunctionAttribution {
public:
	Ink_InterruptSignalTrap interrupt_signal_trap;
	
	Ink_FunctionAttribution()
	: interrupt_signal_trap(DEFAULT_SIGNAL)
	{ }

	Ink_FunctionAttribution(Ink_InterruptSignalTrap trap)
	: interrupt_signal_trap(trap)
	{ }

	inline bool hasTrap(Ink_InterruptSignal sig)
	{
		return hasSignal(interrupt_signal_trap, sig);
	}
};

void Ink_initEnv();
void Ink_removeTmpDir();
void Ink_disposeEnv();

template <class T> T *as(Ink_Expression *obj)
{
	return dynamic_cast<T*>(obj);
}

template <class T> T *as(Ink_Object *obj)
{
	return dynamic_cast<T *>(obj);
}

}

#endif
