#ifndef _UTF8_H_
#define _UTF8_H_

#include <string>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <wchar.h>

namespace ink {

inline size_t
Ink_mbstowcs_len(const char *src)
{
	size_t src_idx, dest_idx;
	size_t status;
	mbstate_t ps;

	memset(&ps, 0, sizeof(mbstate_t));
	for (src_idx = dest_idx = 0; src[src_idx] != '\0'; ) {
		status = mbrtowc(NULL, &src[src_idx], MB_LEN_MAX, &ps);
		if (status < 0) {
			return status;
		}
		dest_idx++;
		src_idx += status;
	}

	return dest_idx;
}

inline size_t
Ink_wcstombs_len(const wchar_t *src)
{
	size_t src_idx, dest_idx;
	size_t status;
	char buf[MB_LEN_MAX];
	mbstate_t ps;

	memset(&ps, 0, sizeof(mbstate_t));
	for (src_idx = dest_idx = 0; src[src_idx] != L'\0'; ) {
		status = wcrtomb(buf, src[src_idx], &ps);
		src_idx++;
		dest_idx += status;
	}

	return dest_idx;
}

inline void
Ink_wcstombs(const wchar_t *src, char *dest)
{
	size_t src_idx, dest_idx;
	size_t status;
	mbstate_t ps;

	memset(&ps, 0, sizeof(mbstate_t));
	for (src_idx = dest_idx = 0; src[src_idx] != '\0'; ) {
		status = wcrtomb(&dest[dest_idx], src[src_idx], &ps);
		src_idx++;
		dest_idx += status;
	}
	dest[dest_idx] = '\0';

	return;
}

inline void
Ink_mbstowcs(const char *src, wchar_t *dest)
{
	size_t src_idx, dest_idx;
	size_t status;
	mbstate_t ps;

	memset(&ps, 0, sizeof(mbstate_t));
	for (src_idx = dest_idx = 0; src[src_idx] != '\0'; ) {
		status = mbrtowc(&dest[dest_idx], &src[src_idx],
						 MB_LEN_MAX, &ps);
		dest_idx++;
		src_idx += status;
	}
	dest[dest_idx] = L'\0';

	return;
}

inline char *
Ink_wcstombs_alloc(const wchar_t *src)
{
	size_t len;
	char *ret;

	len = Ink_wcstombs_len(src);
	ret = (char *)malloc(sizeof(char) * (len + 1));
	Ink_wcstombs(src, ret);

	return ret;
}

inline wchar_t *
Ink_mbstowcs_alloc(const char *src)
{
	int len;
	wchar_t *ret;

	len = Ink_mbstowcs_len(src);
	ret = (wchar_t *)malloc(sizeof(wchar_t) * (len + 1));
	Ink_mbstowcs(src, ret);

	return ret;
}

}

#endif
