#ifndef _BASE_REQ_DEFINE_H_
#define _BASE_REQ_DEFINE_H_

#include "common.h"

#if OS(WINDOWS)
#include "request-win.h"
#else
#include "request-linux.h"
#endif

#define REQ_TYPE_MAP(XX)     \
	XX(CONNECT, connect)     \
	XX(WRITE, write)         \
	XX(READ, read)           \
	XX(ACCEPT, accept)       \
	XX(UDP_WRITE, udp_write) \
	XX(UDP_READ, udp_read)

typedef enum {
  UNKNOWN_REQ = 0,
#define XX(uc, lc) REQ_##uc,
  REQ_TYPE_MAP(XX)
#undef XX
  REQ_TYPE_MAX
} RequestType;

#define REQ_FIELDS          \
	void* data;             \
	RequestType type;       \
	REQ_PRIVATE_FIELDS

typedef struct RequestBase_S
{
	REQ_FIELDS
} RequestBase;

typedef struct AcceptRequest_S
{
	REQ_FIELDS
	ACCEPT_REQ_PRIVATE_FIELDS
	
} AcceptRequest;

typedef struct ReadRequest_S
{
	REQ_FIELDS
} ReadRequest;

typedef struct WriteRequest_S
{
	REQ_FIELDS
	WRITE_PRIVATE_FIELDS
} WriteRequest;

#if OS(WINDOWS)
static struct RequestBase_S* OverlapToReq(OVERLAPPED* overlapped)
{
	return CONTAINING_RECORD(overlapped, struct RequestBase_S, overlapped);
}
#endif

#endif