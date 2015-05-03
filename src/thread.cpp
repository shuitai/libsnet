#include "Thread.h"
#include "Assertions.h"

#if USE(POSIX)
#include <unistd.h>
#endif

#if OS(WINDOWS)
unsigned int __stdcall WorkerThread::Entry(void* argument)
#elif USE(PTHREADS)
void* WorkerThread::Entry(void* argument)
#endif
{
	WorkerThread *pWorkThread = (WorkerThread *)argument;
	(pWorkThread->m_func)(pWorkThread->m_pArg);
	return 0;
}

WorkerThread::WorkerThread(bool bDetach)
	: m_bDetach(bDetach)
{

}

#if OS(WINDOWS)
bool WorkerThread::Start(ThreadRunFunc *func, void *pArg)
{
	m_func = func;
	m_pArg = pArg;
	m_hThreadHandle = (HANDLE)::_beginthreadex(NULL, 0, Entry, reinterpret_cast<void *>(this), 0, NULL);
	if (m_hThreadHandle == 0)
	{
		return false;
	}

	if(m_bDetach)
	{
		::CloseHandle(m_hThreadHandle);
	}
	return true;
}

bool WorkerThread::Stop()
{
	if(m_hThreadHandle == 0 || m_bDetach)
	{
		return false;
	}

	if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hThreadHandle, INFINITE))
	{
		return false;
	}
	
	::CloseHandle(m_hThreadHandle);
	return true;
}

#elif USE(PTHREADS)
bool WorkerThread::Start(ThreadRunFunc *func, void *pArg)
{
	m_func = func;
	m_pArg = pArg;

	if (0 != pthread_create(&m_hThreadHandle, NULL, Entry,  reinterpret_cast<void *>(this)))
	{
		return false;
	}

	if(m_bDetach)
	{
		pthread_detach(m_hThreadHandle);
	}
	return true;
}

bool WorkerThread::Stop()
{
	if(m_hThreadHandle == 0 || m_bDetach)
	{
		return false;
	}

	return pthread_join(m_hThreadHandle, NULL) == 0;
}
#endif

Thread::Thread(bool bDetach) : m_thread(bDetach), m_bRunning(false)
{

}

Thread::~Thread()
{
	Stop();
}

bool Thread::Start()
{
	if (!m_bRunning)
	{
		if (!m_thread.Start(Thread::Entry, this))
		{
			return false;
		}
		m_bRunning = true;
	}

	return true;
}

bool Thread::Stop()
{
	if (m_bRunning)
	{
		m_bRunning = false;
		m_thread.Stop();
	}
	return false;
}

void Thread::Sleep(int milliseconds)
{
#if OS(WINDOWS)
	::Sleep(milliseconds);
#else
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	int ret = nanosleep(&ts, NULL);
	DEBUG_ASSERT(ret == 0);
#endif
}

void Thread::Entry(void* argument)
{
	Thread *pThis = (Thread *)argument;
	pThis->Run();
}


