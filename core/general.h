#ifndef _CORE_GENERAL_H_
#define _CORE_GENERAL_H_

#include <vector>
#include <string>
#include <stdlib.h>
#include "inttype.h"
#include "../includes/universal.h"

#define UNDEFINED (new Ink_Undefined(engine))
#define NULL_OBJ (new Ink_NullObject(engine))
#define TRUE_OBJ (new Ink_Numeric(engine, 1))

#define RETURN_FLAG (engine->CGC_interrupt_signal == INTER_RETURN)
#define BREAK_FLAG (engine->CGC_interrupt_signal == INTER_BREAK)
#define CONTINUE_FLAG (engine->CGC_interrupt_signal == INTER_CONTINUE)
#define DROP_FLAG (engine->CGC_interrupt_signal == INTER_DROP)

#if defined(INK_PLATFORM_LINUX)
namespace ink {
	inline int removeDir(const std::string path)
	{
		return system(("rm -r \"" + path + "\"").c_str());
	}
}

#elif defined(INK_PLATFORM_WIN32)
	#include <stdio.h>
	#include <windows.h>
	#include <conio.h>
namespace ink {

	inline int removeDir(const std::string path, bool if_delete_sub = true)
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
}

#endif

namespace ink {

typedef enum {
	INTER_NONE = 1 << 1,
	INTER_RETURN = 1 << 2,
	INTER_BREAK = 1 << 3,
	INTER_CONTINUE = 1 << 4,
	INTER_DROP = 1 << 5,
	INTER_THROW = 1 << 6,
	INTER_RETRY = 1 << 7
} Ink_InterruptSignal;

class Ink_Expression;
class Ink_Argument;
class Ink_Object;
class Ink_InterpreteEngine;
class Ink_HashTable;
class Ink_Undefined;
class Ink_NullObject;

typedef Ink_UInt64 Ink_InterruptSignalTrap;
typedef std::vector<Ink_Expression *> Ink_ExpressionList;
typedef std::vector<Ink_Argument *> Ink_ArgumentList;

typedef Ink_UInt32 Ink_ArgcType;
typedef Ink_SInt32 IGC_MarkType;
typedef Ink_UInt64 IGC_ObjectCountType;
typedef Ink_SInt64 Ink_LineNoType;
typedef Ink_SInt32 Ink_ExceptionCode;

typedef void (*IGC_Marker)(Ink_InterpreteEngine *engine, Ink_Object *obj);
typedef std::pair<Ink_HashTable *, Ink_HashTable *> IGC_Bonding;
typedef std::vector<IGC_Bonding> IGC_BondingList;

extern Ink_Undefined *ink_global_constant_undefined;
extern Ink_NullObject *ink_global_constant_null;

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

inline bool hasSignal(Ink_InterruptSignalTrap set, Ink_InterruptSignal sign)
{
	return (~(~set | sign) != set);
}

inline Ink_InterruptSignalTrap addSignal(Ink_InterruptSignalTrap set, Ink_InterruptSignal sign)
{
	return set | sign;
}

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
