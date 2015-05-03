#include "iocp-poll.h"
#include "assertions.h"
#include "eventloopadapter.h"

IocpPoll::IocpPoll(EventLoopAdapter* adapter)
	: m_iocp(NULL)
	, m_eventLoopAdapter(adapter)
{
}

IocpPoll::~IocpPoll()
{}

bool IocpPoll::Init()
{
	m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
	if (m_iocp == NULL)
	{
		return false;
	}

	return true;
}

void IocpPoll::Destory()
{
	CloseHandle(m_iocp);
}

void IocpPoll::Poll(DWORD timeout)
{
	DWORD bytes = 0;
	ULONG_PTR key = 0;
	OVERLAPPED* overlapped = NULL;
	RequestBase* req = NULL;

	GetQueuedCompletionStatus(m_iocp, &bytes, &key, &overlapped, timeout);

	if (overlapped) 
	{
		/* Package was dequeued */
		req = OverlapToReq(overlapped);
		// insert req to loop pending queue
		m_eventLoopAdapter->PushToPendingQueue(req);
	}
}