#pragma once

#include "ck/core/platform.h"
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  include <windows.h>
#else
#  include <pthread.h>
#endif

namespace Cki
{


class Mutex
{
public:
    Mutex();
    ~Mutex();

    void lock();
    bool tryLock();
    void unlock();

private:
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    CRITICAL_SECTION m_mutex;
#else
    pthread_mutex_t m_mutex;
#endif

    friend class Cond;

    Mutex(const Mutex&);
    Mutex& operator=(const Mutex&);
};


}
