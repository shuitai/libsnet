#ifndef _MUTEX_H_
#define _MUTEX_H_

#include "common.h"

#if OS(WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#endif

#if USE(PTHREADS)
#include <pthread.h>
#endif

#if USE(PTHREADS)
typedef pthread_mutex_t PlatformMutex;
#elif OS(WINDOWS)
struct PlatformMutex {
	CRITICAL_SECTION m_internalMutex;
	size_t m_recursionCount;
};
#endif

class Mutex
{
public:
	Mutex();
	~Mutex();

	void Lock();
	bool TryLock();
	void UnLock();

public:
	PlatformMutex& impl() { return m_mutex; }

private:
	PlatformMutex m_mutex;
	PLATFORM_NONCOPYABLE(Mutex);
};

class MutexLocker
{
public:
	MutexLocker(Mutex *mutex);
	~MutexLocker();

private:
	Mutex* m_mutex;
	PLATFORM_NONCOPYABLE(MutexLocker);
};

#endif