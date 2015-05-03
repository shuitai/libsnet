#ifndef _BASE_REQUEST_WIN_H_
#define _BASE_REQUEST_WIN_H_

#include "error-inl.h"

#define REQ_PRIVATE_FIELDS             \
	union                              \
	{                                  \
		struct                         \
		{                              \
			OVERLAPPED overlapped;     \
			size_t queuedBytes;        \
		};                             \
	};                                 \
	struct RequestBase_S* next;        \

#define ACCEPT_REQ_PRIVATE_FIELDS      \
	SOCKET acceptSocket;                                          \
	char acceptBuffer[sizeof(struct sockaddr_storage) * 2 + 32];  \
    HANDLE eventHandle;                                           \
    HANDLE waitHandle;                                            \
	struct AcceptRequest_S* nextPending;                          \

#define WRITE_PRIVATE_FIELDS

#define REQ_SUCCESS(req)                                                \
	(NT_SUCCESS(GET_REQ_STATUS((req))))

#define SET_REQ_STATUS(req, status)                                     \
	(req)->overlapped.Internal = (ULONG_PTR) (status)

#define GET_REQ_STATUS(req)                                             \
	((NTSTATUS) (req)->overlapped.Internal)

#define SET_REQ_ERROR(req, error)                                       \
	SET_REQ_STATUS((req), NTSTATUS_FROM_WIN32((error)))

#define GET_REQ_SOCK_ERROR(req)                                         \
	NTStatusToWinsockError((NTSTATUS) ((req)->overlapped.Internal))

#endif