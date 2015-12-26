#ifndef _CORE_GENERAL_H_
#define _CORE_GENERAL_H_

#include <vector>
#include <string>

extern std::vector<std::string *> string_pool;

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
	: name(name)
	{ }

	Ink_Parameter(std::string *name, bool is_ref)
	: name(name), is_ref(is_ref)
	{ }

	Ink_Parameter(std::string *name, bool is_ref, bool is_variant)
	: name(name), is_ref(is_ref), is_variant(is_variant)
	{ }

	Ink_Parameter(std::string *name, bool is_ref, bool is_variant, bool is_optional)
	: name(name), is_ref(is_ref), is_variant(is_variant), is_optional(is_optional)
	{ }
};

typedef enum {
	INTER_NONE = 1 << 1,
	INTER_RETURN = 1 << 2,
	INTER_BREAK = 1 << 3,
	INTER_CONTINUE = 1 << 4
} InterruptSignal;

inline bool hasSignal(int set, InterruptSignal sign)
{
	return (~(~set | sign) != set);
}

inline int addSignal(int set, InterruptSignal sign)
{
	return set | sign;
}

std::string *StrPool_addStr(const char *str);
void StrPool_dispose();

#endif