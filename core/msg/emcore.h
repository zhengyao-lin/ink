#ifndef _EMCORE_H_
#define _EMCORE_H_

#include <sstream>
#include <iostream>
#include <string>
#include <stdarg.h>
#include "../general.h"

namespace ink {

class Ink_ErrorMessage {
public:
	enum Action {
		INK_ERR_ACTION_NONE,
		INK_ERR_ACTION_EXIT0,
		INK_ERR_ACTION_EXIT1,
		INK_ERR_ACTION_ABORT
	};

	enum ErrorLevel {
		INK_ERR_LEVEL_NOTE,
		INK_ERR_LEVEL_WARNING,
		INK_ERR_LEVEL_ERROR
	};

	std::string *file_name;
	Ink_LineNoType line_number;
	std::string *message;

	Ink_ErrorMessage(const char *file_name, Ink_LineNoType line_number, const char *msg, va_list args)
	: file_name(new std::string(file_name)), line_number(line_number)
	{
		message = new std::string(formatMessage(std::string(msg), args));
	}

	void popWith(ErrorLevel level, Action action = INK_ERR_ACTION_NONE, std::ostream& out_s = std::cerr);

	static void doAction(Action action);
	static std::string getLevelPrefix(ErrorLevel level);
	static std::string formatMessage(std::string message, va_list args);

	~Ink_ErrorMessage()
	{
		delete file_name;
		delete message;
	}

};

}

#endif
