#ifndef _CORE_GENERAL_H_
#define _CORE_GENERAL_H_

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

#endif