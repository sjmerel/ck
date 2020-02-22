#pragma once

#include "ck/core/platform.h"
#include "ck/core/mutex.h"

namespace Cki
{


class MutexLock
{
public:
    MutexLock(Mutex& mutex) 
        : m_mutex(&mutex) 
    {
        m_mutex->lock();
    }

    void release()
    {
        if (m_mutex)
        {
            m_mutex->unlock();
            m_mutex = NULL;
        }
    }

    ~MutexLock()
    {
        release();
    }

private:
    Mutex* m_mutex;

    MutexLock(const MutexLock&);
    MutexLock& operator=(const MutexLock&);
};


}
