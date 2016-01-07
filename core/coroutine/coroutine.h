#ifndef _COROUTINE_H_
#define _COROUTINE_H_
#include "../../includes/universal.h"

#if defined(INK_PLATFORM_LINUX)
	#include <stdlib.h>
	#include <ucontext.h>
	#include "../object.h"

	class Ink_UContext: public Ink_Object {
	public:
		ucontext_t *context;

		Ink_UContext(ucontext_t c)
		{
			context = (ucontext_t *)malloc(sizeof(ucontext_t));
			*context = c;
		}

		Ink_UContext()
		: context(NULL)
		{ }

		inline void setContext(ucontext_t c)
		{
			if (!context)
				context = (ucontext_t *)malloc(sizeof(ucontext_t));
			*context = c;
		}

		inline ucontext_t *getContext()
		{ return context; }

		~Ink_UContext()
		{
			if (context)
				free(context);
		}
	};
#endif

#endif