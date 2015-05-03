#ifndef _BASE_EVENT_LOOP_H_
#define _BASE_EVENT_LOOP_H_

#include "common.h"

class MsgHandler;

struct Message
{
	int msgID;
	MsgHandler *pHandler;
	void *pArg;
};

class MsgHandler
{
public:
	virtual ~MsgHandler() {}
	virtual void OnHandleMessage(Message *msg) = 0;

protected:
	MsgHandler() {}

private:
	PLATFORM_NONCOPYABLE(MsgHandler);
};

class EventLoop
{
public:
	virtual ~EventLoop() {}
	static EventLoop* CreateEventLoop();
	static EventLoop* DefaultEventLoop();

	virtual int Start() = 0;
	virtual void Post(const Message& message) = 0;
};

#endif