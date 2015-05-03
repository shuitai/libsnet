#ifndef _BASE_COMMON_H_
#define _BASE_COMMON_H_

#if defined(_MSC_VER) && _MSC_VER < 1600
#include "stdint-msvc2008.h"
#else
#include <stdint.h>
#include <stddef.h>
#endif

#define OS(PLATFORM_FEATURE) (defined PLATFORM_OS_##PLATFORM_FEATURE  && PLATFORM_OS_##PLATFORM_FEATURE)
#define USE(PLATFORM_FEATURE) (defined PLATFORM_USE_##PLATFORM_FEATURE  && PLATFORM_USE_##PLATFORM_FEATURE)

#define PLATFORM_NONCOPYABLE(ClassName) \
	ClassName(const ClassName&); \
	ClassName& operator=(const ClassName&); \

#if DYNAMIC_LIB_EXPORT
#define PLATFORM_EXPORT_DECL __declspec(dllexport)
#define PLATFORM_IMPORT_DECL __declspec(dllimport)
#else
#define PLATFORM_EXPORT_DECL
#define PLATFORM_IMPORT_DECL
#endif

#define ARRAY_SIZE(x) (static_cast<int>(sizeof(x) / sizeof(x[0])))

#endif