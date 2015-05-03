#ifndef _EVENT_LOOP_IMPL_H_
#define _EVENT_LOOP_IMPL_H_

#include "eventloop.h"
#include "request.h"
#include "iocp-poll.h"
#include "thread.h"

class EventLoopAdapter : public EventLoop
{
public:
	EventLoopAdapter();
	virtual ~EventLoopAdapter();

	static EventLoopAdapter* Instance();

	bool Init();
	void Destroy();

	virtual int Start();
	virtual void Post(const Message& message);

	int Loop();
	void PushToPendingQueue(RequestBase* req);

private:
	void ProcessRequest();

private:
	static void ThreadLoop(void*);

private:
	WorkerThread m_workThread;
	RequestBase* m_reqPendingTail;
	bool         m_bStart;

public:
	IocpPoll*    m_iocpPoll;
};

#endif