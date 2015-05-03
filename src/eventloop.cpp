#include "eventloop.h"

#include "eventloopadapter.h"

static EventLoop* s_defaultLoop = NULL;

EventLoop* EventLoop::CreateEventLoop()
{
	return new EventLoopAdapter();
}

EventLoop* EventLoop::DefaultEventLoop()
{
	if (s_defaultLoop == NULL)
	{
		EventLoopAdapter* loopAdapter = EventLoopAdapter::Instance();
		loopAdapter->Init();
		s_defaultLoop = loopAdapter;
	}
	return s_defaultLoop;
}