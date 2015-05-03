#ifndef _BASE_SOCKET_INTERNAL_H_
#define _BASE_SOCKET_INTERNAL_H_

#include "socket.h"

#if OS(WINDOWS)
typedef SOCKET sockfd_t;
#define LastErrorNo WSAGetLastError()

#define HANDLE_READING                       0x00000100
#define HANDLE_BOUND                         0x00000200
#define HANDLE_LISTENING                     0x00000800
#define HANDLE_CONNECTION                    0x00001000
#define HANDLE_CONNECTED                     0x00002000
#define HANDLE_READABLE                      0x00008000
#define HANDLE_WRITABLE                      0x00010000
#define HANDLE_READ_PENDING                  0x00020000

#else
typedef int sockfd_t;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define closesocket(s) close(s)
#define LastErrorNo errno

enum 
{
	CLOSING              = 0x01,   /* uv_close() called but not finished. */
	CLOSED               = 0x02,   /* close(2) finished. */
	STREAM_READING       = 0x04,   /* uv_read_start() called. */
	STREAM_SHUTTING      = 0x08,   /* uv_shutdown() called but not complete. */
	STREAM_SHUT          = 0x10,   /* Write side closed. */
	STREAM_READABLE      = 0x20,   /* The stream is readable */
	STREAM_WRITABLE      = 0x40,   /* The stream is writable */
	STREAM_BLOCKING      = 0x80,   /* Synchronous writes. */
	STREAM_READ_PARTIAL  = 0x100,  /* read(2) read less than requested. */
	STREAM_READ_EOF      = 0x200,  /* read(2) read EOF. */
	TCP_NODELAY          = 0x400,  /* Disable Nagle. */
	TCP_KEEPALIVE        = 0x800,  /* Turn on keep-alive. */
	TCP_SINGLE_ACCEPT    = 0x1000, /* Only accept() when idle. */
	STREAM_CONNECTING    = 0x2000,
	STREAM_CONNECTED     = 0x4000,
	HANDLE_IPV6          = 0x10000 /* Handle is bound to a IPv6 socket. */
};
#endif

const char* inet_ntop_t(int af, const void *src, char* dst, socklen_t size);

int inet_pton_t(int af, const char* src, void *dst);

int setnonblocking(sockfd_t sock, bool nonblock);

#endif