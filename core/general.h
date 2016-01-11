#ifndef _CORE_GENERAL_H_
#define _CORE_GENERAL_H_

#include <vector>
#include <string>
#include <stdlib.h>
#include "../includes/universal.h"

#if defined(INK_PLATFORM_LINUX)

	inline int removeDir(const std::string path)
	{
		return system(("rm -r \"" + path + "\"").c_str());
	}

#elif defined(INK_PLATFORM_WIN32)
	#include <stdio.h>
	#include <windows.h>
	#include <conio.h>

	inline int removeDir(const std::string path, bool if_delete_sub = true)
	{
		bool has_sub_dir = false;
		HANDLE file_handle;
		std::string tmp_file_path;
		std::string pattern;
		WIN32_FIND_DATA file_info;

		pattern = path + "\\*";
		file_handle = ::FindFirstFile(pattern.c_str(), &file_info);
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
					  if (::SetFileAttributes(tmp_file_path.c_str(),
					                         FILE_ATTRIBUTE_NORMAL) == false)
					    return ::GetLastError();

					  if (::DeleteFile(tmp_file_path.c_str()) == false)
					    return ::GetLastError();
					}
				}
			} while (::FindNextFile(file_handle, &file_info) == true);

			::FindClose(file_handle);

			DWORD dwError = ::GetLastError();
			if (dwError != ERROR_NO_MORE_FILES)
				return dwError;
			else {
				if (!has_sub_dir) {
					if (::SetFileAttributes(path.c_str(),
										   FILE_ATTRIBUTE_NORMAL) == false)
					return ::GetLastError();

					if (::RemoveDirectory(path.c_str()) == false)
						return ::GetLastError();
				}
			}
		}

		return 0;
	}

#endif

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

class Ink_Expression;

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

typedef enum {
	INTER_NONE = 1 << 1,
	INTER_RETURN = 1 << 2,
	INTER_BREAK = 1 << 3,
	INTER_CONTINUE = 1 << 4,
	INTER_DROP = 1 << 5,
	INTER_THROW = 1 << 6,
	INTER_YIELD = 1 << 7
} InterruptSignal;

inline bool hasSignal(int set, InterruptSignal sign)
{
	return (~(~set | sign) != set);
}

inline int addSignal(int set, InterruptSignal sign)
{
	return set | sign;
}

class Ink_Object;
extern InterruptSignal CGC_interrupt_signal;
extern Ink_Object *CGC_interrupt_value;

inline Ink_Object *trapSignal()
{
	Ink_Object *tmp = CGC_interrupt_value;
	CGC_interrupt_signal = INTER_NONE;
	CGC_interrupt_value = NULL;
	return tmp;
}

std::string *StrPool_addStr(const char *str);
std::string *StrPool_addStr(std::string *str);
void StrPool_dispose();

#endif