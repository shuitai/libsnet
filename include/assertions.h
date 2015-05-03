#ifndef _BASE_ASSERTIONS_H_
#define _BASE_ASSERTIONS_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef NDEBUG
#define ASSERTIONS_DISABLED_DEFAULT 1
#else
#define ASSERTIONS_DISABLED_DEFAULT 0
#endif

#ifndef ASSERT_DISABLED
#define ASSERT_DISABLED ASSERTIONS_DISABLED_DEFAULT
#endif

#ifdef __cplusplus
extern "C" {
#endif
	void ReportAssertionFailure(const char* file, int line, const char* function, const char* assertion);
	void PlatformCrash();
#ifdef __cplusplus
}
#endif

#ifndef CRASH
#define CRASH() PlatformCrash()
#endif

#if ASSERT_DISABLED
#define DEBUG_ASSERT(assertion) ((void)0)
#else
#define DEBUG_ASSERT(assertion) \
	(!(assertion) ? \
	(ReportAssertionFailure(__FILE__, __LINE__, __FUNCTION__, #assertion), \
	CRASH()) : \
	(void)0)
#endif

#endif