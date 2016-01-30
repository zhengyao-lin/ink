#include <sstream>
#include <iostream>
#include <string>
#include <string>
#include "emcore.h"
#include "../../includes/universal.h"

#if defined(INK_PLATFORM_LINUX)

	#define INK_COLOR_NONE         "\033[m"
	#define INK_COLOR_BOLD         "\033[1m"
	#define INK_COLOR_RED          "\033[0;32;31m"
	#define INK_COLOR_LIGHT_RED    "\033[1;31m"
	#define INK_COLOR_GREEN        "\033[0;32;32m"
	#define INK_COLOR_LIGHT_GREEN  "\033[1;32m"
	#define INK_COLOR_BLUE         "\033[0;32;34m"
	#define INK_COLOR_LIGHT_BLUE   "\033[1;34m"
	#define INK_COLOR_DARY_GRAY    "\033[1;30m"
	#define INK_COLOR_CYAN         "\033[0;36m"
	#define INK_COLOR_LIGHT_CYAN   "\033[1;36m"
	#define INK_COLOR_PURPLE       "\033[0;35m"
	#define INK_COLOR_LIGHT_PURPLE "\033[1;35m"
	#define INK_COLOR_BROWN        "\033[0;33m"
	#define INK_COLOR_YELLOW       "\033[1;33m"
	#define INK_COLOR_LIGHT_GRAY   "\033[0;37m"
	#define INK_COLOR_WHITE        "\033[1;37m"
	#define INK_COLOR_DEFAULT      "\033[0m"
	#define INK_COLOR_NEXT_LINE    "\n"
	#define INK_COLOR_CLOSE_COLOR  "\033[0m\n"

#else

	#define INK_COLOR_NONE         ""
	#define INK_COLOR_BOLD         ""
	#define INK_COLOR_RED          ""
	#define INK_COLOR_LIGHT_RED    ""
	#define INK_COLOR_GREEN        ""
	#define INK_COLOR_LIGHT_GREEN  ""
	#define INK_COLOR_BLUE         ""
	#define INK_COLOR_LIGHT_BLUE   ""
	#define INK_COLOR_DARY_GRAY    ""
	#define INK_COLOR_CYAN         ""
	#define INK_COLOR_LIGHT_CYAN   ""
	#define INK_COLOR_PURPLE       ""
	#define INK_COLOR_LIGHT_PURPLE ""
	#define INK_COLOR_BROWN        ""
	#define INK_COLOR_YELLOW       ""
	#define INK_COLOR_LIGHT_GRAY   ""
	#define INK_COLOR_WHITE        ""
	#define INK_COLOR_DEFAULT      ""
	#define INK_COLOR_NEXT_LINE    ""
	#define INK_COLOR_CLOSE_COLOR  ""

#endif

namespace ink {

using namespace std;

void Ink_ErrorMessage::popWith(ErrorLevel level, Action action, ostream& out_s)
{
	stringstream strm;
	strm << getLevelPrefix(level) << " "
		 << file_name->c_str() << ": "
		 << "line " << line_number << ": "
		 << message->c_str();

	out_s << strm.str() << endl;
	doAction(action);

	return;
}

void Ink_ErrorMessage::doAction(Action action)
{
	switch (action) {
		case INK_ERR_ACTION_EXIT0:
			exit(0);
		case INK_ERR_ACTION_EXIT1:
			exit(1);
		case INK_ERR_ACTION_ABORT:
			abort();
		default: ;
	}
	return;
}

string Ink_ErrorMessage::getLevelPrefix(ErrorLevel level)
{
	switch (level) {
		case INK_ERR_LEVEL_NOTE:
			return INK_COLOR_LIGHT_BLUE + string("***NOTE***") + INK_COLOR_NONE;
		case INK_ERR_LEVEL_WARNING:
			return INK_COLOR_YELLOW + string("***WARNING***") + INK_COLOR_NONE;
		case INK_ERR_LEVEL_ERROR:
			return INK_COLOR_LIGHT_RED + string("***ERROR***") + INK_COLOR_NONE;
	}
	return "";
}

string Ink_ErrorMessage::formatMessage(string message, va_list args)
{
	string::size_type i, length;
	string tmp = string(message);

	for (i = 0; i < tmp.length() && tmp.c_str()[i] != '\0'; i++)
	{
		if (tmp.c_str()[i] != '$') {
			continue;
		} else if (tmp.c_str()[i + 1] != '(') {
			continue;
		}

		for (length = 1; tmp.c_str()[i + length] != ')'; length++);
		tmp.replace(i, length + 1, va_arg(args, const char *));
		i += length;
	}

	return tmp;
}

}
