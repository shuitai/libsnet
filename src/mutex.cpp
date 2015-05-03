#include "Mutex.h"
#include "Assertions.h"

#if OS(WINDOWS)
Mutex::Mutex()
{
	m_mutex.m_recursionCount = 0;
	InitializeCriticalSection(&m_mutex.m_internalMutex);
}

Mutex::~Mutex()
{
	DeleteCriticalSection(&m_mutex.m_internalMutex);
}

void Mutex::Lock()
{
	EnterCriticalSection(&m_mutex.m_internalMutex);
	++m_mutex.m_recursionCount;
}

bool Mutex::TryLock()
{
	DWORD result = TryEnterCriticalSection(&m_mutex.m_internalMutex);

	if (result != 0) 
	{
		if (m_mutex.m_recursionCount > 0) 
		{
			LeaveCriticalSection(&m_mutex.m_internalMutex);
			return false;
		}

		++m_mutex.m_recursionCount;
		return true;
	}

	return false;
}

void Mutex::UnLock()
{
	--m_mutex.m_recursionCount;
	LeaveCriticalSection(&m_mutex.m_internalMutex);
}

#elif USE(PTHREADS)
Mutex::Mutex()
{
	pthread_mutex_init(&m_mutex, NULL);
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_mutex);
}

void Mutex::Lock()
{
	pthread_mutex_lock(&m_mutex);
}

bool Mutex::TryLock()
{
	int result = pthread_mutex_trylock(&m_mutex);
	return result == 0;
}

void Mutex::UnLock()
{
	pthread_mutex_unlock(&m_mutex);
}
#endif


MutexLocker::MutexLocker(Mutex *mutex)
	: m_mutex(mutex)
{
	DEBUG_ASSERT(m_mutex != NULL);
	m_mutex->Lock();
}

MutexLocker::~MutexLocker()
{
	m_mutex->UnLock();
}