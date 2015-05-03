#include "eventloopadapter.h"
#include "assertions.h"
#include "socket-tcp.h"
#include "socket-udp.h"

static EventLoopAdapter* s_loopAdapter = NULL;

EventLoopAdapter::EventLoopAdapter()
	: m_reqPendingTail(NULL)
	, m_iocpPoll(NULL)
	, m_bStart(false)
{
	m_iocpPoll = new IocpPoll(this);
}

EventLoopAdapter::~EventLoopAdapter()
{
	delete m_iocpPoll;
}

EventLoopAdapter* EventLoopAdapter::Instance()
{
	if (s_loopAdapter == NULL)
	{
		WSADATA wsad;
		if (WSAStartup(MAKEWORD(2, 2), &wsad) )
		{
			return NULL;
		}

		s_loopAdapter = new EventLoopAdapter;
	}

	return s_loopAdapter;
}

bool EventLoopAdapter::Init()
{
	if (m_bStart)
	{
		return false;
	}

	bool ret = m_iocpPoll->Init();
	m_bStart = ret;
	return ret;
}

void EventLoopAdapter::Destroy()
{
	if (!m_bStart)
	{
		return;
	}
	m_iocpPoll->Destory();
}

int EventLoopAdapter::Start()
{
	return m_workThread.Start(ThreadLoop, this);
}

int EventLoopAdapter::Loop()
{
	while(m_bStart)
	{
		ProcessRequest();
		m_iocpPoll->Poll(1000);
	}

	printf("Exit EventLoopAdapter::Loop()!");

	return 0;
}

void EventLoopAdapter::Post(const Message& message)
{
}

void EventLoopAdapter::PushToPendingQueue(RequestBase* req)
{
	if (m_reqPendingTail)
	{
		req->next = m_reqPendingTail->next;
		m_reqPendingTail->next = req;
		m_reqPendingTail = req;
	}
	else
	{
		req->next = req;
		m_reqPendingTail = req;
	}
}

void EventLoopAdapter::ProcessRequest()
{
	RequestBase* req = NULL;
	RequestBase* firstReq = NULL;
	RequestBase* nextReq = NULL;
	if (m_reqPendingTail == NULL)
	{
		return;
	}

	firstReq = m_reqPendingTail->next;
	nextReq = firstReq;

	m_reqPendingTail = NULL;
	
	while (nextReq != NULL)
	{
		req = nextReq;
		nextReq = req->next != firstReq ? req->next : NULL;

		switch(req->type)
		{
		case REQ_READ:
			((TCPSocket*)req->data)->ReadCallback((ReadRequest*)req);
			break;
		case REQ_WRITE:
			((TCPSocket*)req->data)->WriteCallback((WriteRequest*)req);
			break;
		case REQ_CONNECT:
			((TCPSocket*)req->data)->ConnectionCallback(req);
			break;
		case REQ_ACCEPT:
			((TCPServerSocket*)req->data)->ListenCallback((AcceptRequest*)req);
			break;
		case REQ_UDP_WRITE:
			((UDPSocket*)req->data)->WriteCallback((WriteRequest*)req);
			break;
		case REQ_UDP_READ:
			((UDPSocket*)req->data)->ReadCallback((ReadRequest*)req);
			break;
		default:
			DEBUG_ASSERT(0);
			break;
		}
	}
}

void EventLoopAdapter::ThreadLoop(void* args)
{
	((EventLoopAdapter*)args)->Loop();
}