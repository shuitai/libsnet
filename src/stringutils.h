#ifndef _BASE_STRINGUTILS_H_
#define _BASE_STRINGUTILS_H_

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace string_base 
{

template<class CTYPE>
size_t vsprintfn(CTYPE* buffer, size_t buflen, const CTYPE* format, va_list args) 
{
	int len = vsnprintf(buffer, buflen, format, args);
	if ((len < 0) || (static_cast<size_t>(len) >= buflen)) 
	{
		len = static_cast<int>(buflen - 1);
		buffer[len] = 0;
	}
	return len;
}

template<class CTYPE>
size_t sprintfn(CTYPE* buffer, size_t buflen, const CTYPE* format, ...);
template<class CTYPE>
size_t sprintfn(CTYPE* buffer, size_t buflen, const CTYPE* format, ...) 
{
	va_list args;
	va_start(args, format);
	size_t len = vsprintfn(buffer, buflen, format, args);
	va_end(args);
	return len;
}

template<class CTYPE>
const CTYPE* strchr(const CTYPE* str, const CTYPE* chs) {
	for (size_t i=0; str[i]; ++i) 
	{
		for (size_t j=0; chs[j]; ++j) 
		{
			if (str[i] == chs[j])
			{
				return str + i;
			}
		}
	}
	return 0;
}

}

#endif