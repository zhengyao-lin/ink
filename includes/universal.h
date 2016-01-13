#ifndef _UNIVERSAL_H_
#define _UNIVERSAL_H_

#if defined(__linux__)
	#define INK_PLATFORM_LINUX
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
	#define INK_PLATFORM_WIN32
	#define INK_PLATFORM_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
	#define INK_PLATFORM_WIN32
#endif

#endif
