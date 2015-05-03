#include "Assertions.h"
#include "common.h"

#if OS(WINDOWS)
#include <crtdbg.h>
#endif

extern "C" 
{
void ReportAssertionFailure(const char* file, int line, const char* function, const char* assertion)
{
#if OS(WINDOWS) && !OS(WINCE) && defined(_DEBUG)
	_CrtDbgReport(_CRT_WARN, file, line, NULL, "%s\n", function);
#else
	fprintf(stderr, "%s(%d) : %s\n", file, line, function);
#endif
}

void PlatformCrash()
{
	abort();
}

}