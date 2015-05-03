#include "socket-inl.h"

#if OS(WINDOWS)
//#if (NTDDI_VERSION <= NTDDI_VISTA)
#include "win32helper.h"
//#endif
#else
#include <fcntl.h>
#endif

const char* inet_ntop_t(int af, const void *src, char* dst, socklen_t size)
{
#if OS(WINDOWS)
	/*#if (NTDDI_VERSION >= NTDDI_VISTA)
	return inet_ntop(af, (void*)src, dst, size);
	#else*/
	return win32_inet_ntop(af, src, dst, size);
	//#endif
#else
	return ::inet_ntop(af, (void*)src, dst, size);
#endif
}

int inet_pton_t(int af, const char* src, void *dst)
{
#if OS(WINDOWS)
	/*#if (NTDDI_VERSION >= NTDDI_VISTA)
	return inet_pton(af, src, dst);
	#else*/
	return win32_inet_pton(af, src, dst);
	//#endif
#else
	return ::inet_pton(af, src, dst);
#endif
}

int setnonblocking(sockfd_t sock, bool nonblock)
{
#if OS(WINDOWS)
	unsigned long nonblockValue = nonblock ? 1 : 0;
    return ::ioctlsocket (sock, FIONBIO, &nonblockValue);
#else
	int flags = fcntl(sock, F_GETFL, 0);
	if(nonblock)
	{
		flags |= O_NONBLOCK;
	}
	else
	{
		flags &= ~O_NONBLOCK;
	}

	return fcntl(sock, F_SETFL, flags);
#endif
}