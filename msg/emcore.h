#ifndef _EMCORE_H_
#define _EMCORE_H_

#include <fstream>
#include <sstream>
#include <iostream>
#include <queue>
#include <string>
#include "stdarg.h"

#define NONE         "\033[m"
#define BOLD         "\033[1m"
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"
#define DEFAULT      "\033[0m"
#define NEXT_LINE    "\n"
#define CLOSE_COLOR  "\033[0m\n"

using namespace std;

class ErrorInfo {
public:
	enum ActionFlag {
		NoAct,
		Exit0,
		Exit1,
		Abort
	};
	enum Prefix {
		None,
		Note,
		Warning,
		Error
	};

private:
	string MSG;
	ActionFlag AF;

	string
	setMessageStyle(string message, Prefix prefix);

	string
	setMessageStyle(string message, Prefix prefix, bool is_bold);

	string
	createFormatMessage(string message, va_list args);

public:
	ErrorInfo(string message, ...);

	ErrorInfo(ActionFlag action, string message, ...);

	ErrorInfo(Prefix prefix, string message, ...);

	ErrorInfo(Prefix prefix, bool is_bold, string message, ...);

	ErrorInfo(Prefix prefix, ActionFlag action, string message, ...);

	ErrorInfo(Prefix prefix, bool is_bold, ActionFlag action, string message, ...);

	ActionFlag
	getActionFlag();

	void
	setActionFlag(ActionFlag action);

	void
	doPrint(ostream& strm);

	void
	doPrint(ostream& strm, ActionFlag action);

	void
	addPrefix(string prefix);
};

class ErrorMessage {
	queue<ErrorInfo *> Buffer;

public:
	ErrorMessage() { }

	void
	newMessage(ErrorInfo *info);

	void
	popAll(ostream& strm);

	void
	popInDefaultAction(ostream& strm);

	void
	popAllAndExit1(ostream& strm);

	void
	setTopLineNumber(int line_number);

	void
	setTopFileName(char *file_name);

	static void
	tmpError(string msg);
	static void
	tmpWarning(string msg);
	static void
	tmpNote(string msg);
	static void
	popMessage(ErrorInfo *info);
};

#endif
