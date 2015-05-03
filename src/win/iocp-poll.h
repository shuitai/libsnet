#ifndef _BASE_IOCP_POLL_H_
#define _BASE_IOCP_POLL_H_

#include "request.h"

class EventLoopAdapter;

class IocpPoll
{
public:
	IocpPoll(EventLoopAdapter* adapter);
	~IocpPoll();

	bool Init();
	void Destory();
	void Poll(DWORD timeout);	

public:
	HANDLE m_iocp;

private:
	EventLoopAdapter* m_eventLoopAdapter;
};

#endif