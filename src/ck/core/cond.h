#pragma once

#include "ck/core/platform.h"
#include "ck/core/mutex.h"
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  include <windows.h>
#else
#  include <pthread.h>
#endif

namespace Cki
{

// Usage:
//
// thread 1:
//   m_mutex.lock();
//   if (m_theresWorkToDo)
//   {
//      // do some work
//   }
//   else
//   {
//      m_cond.wait(m_mutex);
//   }
//   m_mutex.unlock();
//
// thread 2:
//   m_mutex.lock();
//   m_theresWorkToDo = isThereWorkToDo();
//   if (m_theresWorkToDo)
//   {
//      m_cond.signal();
//   }
//   m_mutex.unlock();
//


class Cond
{
public:
    Cond();
    ~Cond();

    void wait(Mutex&);
    void signal();

private:
#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
    // based on http://www.cse.wustl.edu/~schmidt/win32-cv-1.html
    int m_waiters;
    Mutex m_waitersMutex;
    HANDLE m_event;
#else
    pthread_cond_t m_cond;
#endif

    Cond(const Cond&);
    Cond& operator=(const Cond&);
};


}

