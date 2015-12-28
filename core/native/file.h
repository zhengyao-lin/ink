#ifndef _NATIVE_FILE_H
#define _NATIVE_FILE_H

#include <stdio.h>
#include "../object.h"

#define FILE_GETS_BUFFER_SIZE 1000

extern Ink_TypeTag file_pointer_type_tag;

class Ink_FilePointer: public Ink_Object {
public:
	FILE *fp;

	Ink_FilePointer(FILE *fp = NULL)
	: fp(fp)
	{
		type = file_pointer_type_tag;
		setMethod();
	}

	void setMethod();
};

#endif