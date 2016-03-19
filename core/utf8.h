#ifndef _UTF8_H_
#define _UTF8_H_

#include <string>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <wchar.h>

namespace ink {

size_t Ink_mbstowcs_len(const char *src);
size_t Ink_wcstombs_len(const wchar_t *src);

void Ink_wcstombs(const wchar_t *src, char *dest);
void Ink_mbstowcs(const char *src, wchar_t *dest);

char *Ink_wcstombs_alloc(const wchar_t *src);
wchar_t *Ink_mbstowcs_alloc(const char *src);

char *Ink_mbstoupper_alloc(const char *src);

}

#endif
