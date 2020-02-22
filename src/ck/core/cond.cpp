#include "ck/core/cond.h"
#include "ck/core/debug.h"

namespace Cki
{


Cond::Cond()
{
#if CK_PLATFORM_WIN 
    m_waiters = 0;
    m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    CK_ASSERT(m_event);
#elif CK_PLATFORM_WP8
    m_waiters = 0;
    m_event = CreateEventEx(NULL, NULL, 0, SYNCHRONIZE | EVENT_MODIFY_STATE | DELETE);
    CK_ASSERT(m_event);
#else
    CK_VERIFY( 0 == pthread_cond_init(&m_cond, NULL) );
#endif
}

Cond::~Cond()
{
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    CK_VERIFY( CloseHandle(m_event) );
#else
    CK_VERIFY( 0 == pthread_cond_destroy(&m_cond) );
#endif
}

void Cond::wait(Mutex& mutex)
{
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    m_waitersMutex.lock();
    ++m_waiters;
    m_waitersMutex.unlock();

    mutex.unlock();

#if CK_PLATFORM_WIN 
    CK_VERIFY( WaitForSingleObject(m_event, INFINITE) == WAIT_OBJECT_0 );
#else // CK_PLATFORM_WP8
	CK_VERIFY( WaitForSingleObjectEx(m_event, INFINITE, FALSE) == WAIT_OBJECT_0 );
#endif

    m_waitersMutex.lock();
    --m_waiters;
    m_waitersMutex.unlock();

    mutex.lock();
#else
    CK_VERIFY( 0 == pthread_cond_wait(&m_cond, &mutex.m_mutex) );
#endif
}

void Cond::signal()
{
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    m_waitersMutex.lock();
    int waiters = m_waiters;
    m_waitersMutex.unlock();

    if (waiters > 0)
    {
        CK_VERIFY( SetEvent(m_event) );
    }
#else
    CK_VERIFY( 0 == pthread_cond_signal(&m_cond) );
#endif
}


}
