#ifndef _BASE_THREAD_H_
#define _BASE_THREAD_H_

#include "common.h"

#if USE(PTHREADS)
#include <pthread.h>
typedef pthread_t ThreadHandle;
#elif OS(WINDOWS)
#include <windows.h>
#include <process.h>
typedef HANDLE ThreadHandle;
#endif

typedef void (ThreadRunFunc)(void*);

class WorkerThread
{
public:
	WorkerThread(bool bDetach = false);
	bool Start(ThreadRunFunc *func, void *pArg);
	bool Stop();

private:
#if OS(WINDOWS)
	static unsigned int __stdcall Entry(void* argument);
#elif USE(PTHREADS)
	static void *Entry(void *argument);
#endif

private:
	ThreadHandle m_hThreadHandle;
	ThreadRunFunc *m_func;
	void *m_pArg;
	bool m_bDetach;
};

class Thread
{
public:
	Thread(bool bDetach = false);
	virtual ~Thread();
	bool Start();
	bool Stop();

	/**************************************************************************
	*@Function:sleep
	*@Brief   :Sleeps for specified number of millisecond.
	*@Params  :seconds [in] sleep seconds.
	*@Return  :None.
	*
	**************************************************************************/
	static void Sleep(int milliseconds); 

	bool IsRunning()
	{
		return m_bRunning;
	}
	
	virtual void Run() = 0;

private:
	static void Entry(void *pArg);

private:
	WorkerThread m_thread;
	bool m_bDetach;
	bool m_bRunning;
	
};
#endif