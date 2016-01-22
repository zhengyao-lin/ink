#include <iostream>
#include <stdlib.h>
#include "emcore.h"
#include "../thread/thread.h"

string
ErrorInfo::setMessageStyle(string message, Prefix prefix)
{
	switch (prefix) {
		case None:
			break;
		case Note:
			message = LIGHT_BLUE + string("Note: ") + NONE + message;
			break;
		case Warning:
			message = YELLOW + string("Warning: ") + NONE + message;
			break;
		case Error:
			message = LIGHT_RED + string("Error: ") + NONE + message;
			break;
	}

	return message;
}

string
ErrorInfo::setMessageStyle(string message, Prefix prefix, bool is_bold)
{
	switch (prefix) {
		case None:
			message = (is_bold ? BOLD : NONE) + message + NONE;
			break;
		case Note:
			message = LIGHT_BLUE + string("Note: ") + NONE + (is_bold ? BOLD : NONE) + message + NONE;
			break;
		case Warning:
			message = YELLOW + string("Warning: ") + NONE + (is_bold ? BOLD : NONE) + message + NONE;
			break;
		case Error:
			message = LIGHT_RED + string("Error: ") + NONE + (is_bold ? BOLD : NONE) + message + NONE;
			break;
	}

	return message;
}

string
ErrorInfo::createFormatMessage(string message, va_list args)
{
	unsigned int i, length;

	for (i = 0; i < message.size() && message.c_str()[i] != '\0'; i++)
	{
		if (message.c_str()[i] != '$') {
			continue;
		} else if (message.c_str()[i + 1] != '(') {
			continue;
		}

		for (length = 1; message.c_str()[i + length] != ')'; length++);
		message.replace(i, length + 1, va_arg(args, char *));
		i += length;
	}

	return message;
}

ErrorInfo::ErrorInfo(string message, ...)
: AF(NoAct)
{
	va_list args;

	va_start(args, message);
	MSG = createFormatMessage(message, args);
	va_end(args);

	return;
}

ErrorInfo::ErrorInfo(ActionFlag action, string message, ...)
: AF(action)
{
	va_list args;

	va_start(args, message);
	MSG = createFormatMessage(message, args);
	va_end(args);

	return;
}

ErrorInfo::ErrorInfo(Prefix prefix, string message, ...)
: AF(NoAct)
{
	va_list args;

	va_start(args, message);
	MSG = createFormatMessage(message, args);
	va_end(args);

	return;
}

ErrorInfo::ErrorInfo(Prefix prefix, bool is_bold, string message, ...)
: AF(NoAct)
{
	va_list args;

	va_start(args, message);
	MSG = setMessageStyle(createFormatMessage(message, args),
						  prefix, is_bold);
	va_end(args);

	return;
}

ErrorInfo::ErrorInfo(Prefix prefix, ActionFlag action, string message, ...)
: AF(action)
{
	va_list args;

	va_start(args, message);
	MSG = setMessageStyle(createFormatMessage(message, args),
						  prefix);
	va_end(args);

	return;
}

ErrorInfo::ErrorInfo(Prefix prefix, bool is_bold, ActionFlag action, string message, ...)
: AF(action)
{
	va_list args;

	va_start(args, message);
	MSG = setMessageStyle(createFormatMessage(message, args),
						  prefix, is_bold);
	va_end(args);

	return;
}

ErrorInfo::ActionFlag
ErrorInfo::getActionFlag()
{
	return AF;
}

void
ErrorInfo::setActionFlag(ActionFlag action)
{
	AF = action;
	return;
}

void
ErrorInfo::doPrint(ostream& strm)
{
	strm << MSG << endl;
	switch (AF) {
		case NoAct:
			break;
		case Exit0:
			exit(0);
			break;
		case Exit1:
			exit(1);
			break;
		case Abort:
			abort();
			break;
		case ExitThread:
			pthread_exit(NULL);
			break;
	}

	return;
}

void
ErrorInfo::doPrint(ostream& strm, ActionFlag action)
{
	strm << MSG << endl;
	switch (action) {
		case NoAct:
			break;
		case Exit0:
			exit(0);
			break;
		case Exit1:
			exit(1);
			break;
		case Abort:
			abort();
			break;
		case ExitThread:
			pthread_exit(NULL);
			break;
	}

	return;
}

void
ErrorInfo::addPrefix(string prefix)
{
	MSG = prefix + MSG;
	return;
}

void
ErrorMessage::newMessage(ErrorInfo *info)
{
	Buffer.push(info);
	return;
}

void
ErrorMessage::popMessage(ErrorInfo info)
{
	info.doPrint(cerr);
	return;
}

void
ErrorMessage::popAll(ostream& strm)
{
	while (Buffer.size() > 0)
	{
		Buffer.front()->doPrint(strm, ErrorInfo::NoAct);
		delete Buffer.front();
		Buffer.pop();
	}

	return;
}

void
ErrorMessage::popInDefaultAction(ostream& strm)
{
	while (Buffer.size() > 0)
	{
		Buffer.front()->doPrint(strm);
		delete Buffer.front();
		Buffer.pop();
	}

	return;
}

void
ErrorMessage::popAllAndExit1(ostream& strm)
{
	while (Buffer.size() > 0)
	{
		Buffer.front()->doPrint(strm);
		delete Buffer.front();
		Buffer.pop();
	}

	exit(1);
	return;
}

void
ErrorMessage::setTopLineNumber(int line_number)
{
	stringstream strm;
	strm << line_number << ": ";

	Buffer.front()->addPrefix(strm.str());
	Buffer.front()->addPrefix("line ");
	return;
}

void
ErrorMessage::setTopFileName(char *file_name)
{
	Buffer.front()->addPrefix(string(file_name) + ": ");
	return;
}

void
ErrorMessage::tmpError(string msg)
{
	ErrorInfo *err = new ErrorInfo(ErrorInfo::Error, true,
								   ErrorInfo::Exit1, msg);
	err->doPrint(cerr);
	delete err;
	return;
}

void
ErrorMessage::tmpWarning(string msg)
{
	ErrorInfo *warn = new ErrorInfo(ErrorInfo::Warning, true,
									ErrorInfo::NoAct, msg);
	warn->doPrint(cerr);
	delete warn;
	return;
}

void
ErrorMessage::tmpNote(string msg)
{
	ErrorInfo *note = new ErrorInfo(ErrorInfo::Note, true,
									ErrorInfo::NoAct, msg);
	note->doPrint(cerr);
	delete note;
	return;
}
