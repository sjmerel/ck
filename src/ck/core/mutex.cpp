#include "ck/core/mutex.h"
#include "ck/core/debug.h"
#include <errno.h>

namespace Cki
{


Mutex::Mutex()
{
#if CK_PLATFORM_WIN 
    InitializeCriticalSection(&m_mutex);
#elif CK_PLATFORM_WP8
    InitializeCriticalSectionEx(&m_mutex, 0, 0);
#else
    // allow recursive locking (matches critical section behavior on Windows)
    pthread_mutexattr_t attr;
    CK_VERIFY( 0 == pthread_mutexattr_init(&attr) );
    CK_VERIFY( 0 == pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) );

    CK_VERIFY( 0 == pthread_mutex_init(&m_mutex, &attr) );

    CK_VERIFY( 0 == pthread_mutexattr_destroy(&attr) );
#endif
}

Mutex::~Mutex()
{
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    DeleteCriticalSection(&m_mutex);
#else
    CK_VERIFY( 0 == pthread_mutex_destroy(&m_mutex) );
#endif
}

void Mutex::lock()
{
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    EnterCriticalSection(&m_mutex);
#else
    CK_VERIFY( 0 == pthread_mutex_lock(&m_mutex) );
#endif
}

bool Mutex::tryLock()
{
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    return (TryEnterCriticalSection(&m_mutex) == TRUE);
#else
    int result = pthread_mutex_trylock(&m_mutex);
    CK_ASSERT(result == 0 || result == EBUSY);
    return (result == 0);
#endif
}

void Mutex::unlock()
{
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    LeaveCriticalSection(&m_mutex);
#else
    CK_VERIFY( 0 == pthread_mutex_unlock(&m_mutex) );
#endif
}


}
