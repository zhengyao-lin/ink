#ifndef _NATIVE_FILE_H
#define _NATIVE_FILE_H

#include <stdio.h>
#include "../object.h"

#define FILE_GETS_BUFFER_SIZE 1000

class Ink_FilePointer: public Ink_Object {
public:
	FILE *fp;

	Ink_FilePointer(FILE *fp = NULL)
	: fp(fp)
	{
		type = INK_CUSTOM;
		setMethod();
	}

	void setMethod();
};

#endif