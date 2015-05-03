#ifndef _BASE_WIN32HELPER_H_
#define _BASE_WIN32HELPER_H_

#include "common.h"

#if OS(WINDOWS)
typedef int socklen_t;
#endif

#if OS(WINDOWS)
const char* win32_inet_ntop(int af, const void *src, char* dst, socklen_t size);
int win32_inet_pton(int af, const char* src, void *dst);
#endif

#endif