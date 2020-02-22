#include "ck/core/taskqueue.h"
#include "ck/core/mutexlock.h"
#include "ck/core/mem.h"
#include "ck/core/debug.h"

namespace
{
    enum { k_fillByte = 0xab };
}

namespace Cki
{


template <typename T>
TaskQueue<T>::TaskQueue(int capacity) :
    m_capacity(capacity),
    m_read(0),
    m_write(0)
{
    m_tasks = (T*) Mem::alloc(sizeof(T) * capacity);
#if CK_DEBUG
    Mem::fill(m_tasks, k_fillByte, sizeof(T) * capacity);
#endif
}

template <typename T>
TaskQueue<T>::~TaskQueue()
{
    Mem::free(m_tasks);
}

template <typename T>
bool TaskQueue<T>::produce(const T& task) 
{
    MutexLock lock(m_mutex);
    /*
#if CK_DEBUG
    static int s_produceThread = -1;
    if (s_produceThread == -1)
    {
        s_produceThread = Thread::getCurrentThreadId();
    }
    CK_ASSERT(Thread::getCurrentThreadId() == s_produceThread);
#endif
*/

    int nextWrite = next(m_write);
    bool grown = false;

    if (nextWrite == m_read)
    {
        // reallocate task buffer
        // TODO: limit its size
        int newCapacity = m_capacity * 2;
        T* newTasks = (T*) Mem::alloc(sizeof(T) * newCapacity);
#if CK_DEBUG
        Mem::fill(newTasks, k_fillByte, sizeof(T) * newCapacity);
#endif

        int count = 0;
        if (m_read < m_write)
        {
            count = m_write - m_read;
            Mem::copy(newTasks, m_tasks + m_read, sizeof(T) * count);
        }
        else if (m_read > m_write)
        {
            count = m_capacity - (m_read - m_write);
            Mem::copy(newTasks, m_tasks + m_read, sizeof(T) * (m_capacity - m_read));
            Mem::copy(newTasks + (m_capacity - m_read), m_tasks, sizeof(T) * m_write);
        }

        Mem::free(m_tasks);

        m_tasks = newTasks;
        m_capacity = newCapacity;
        m_read = 0;
        m_write = count;
        nextWrite = next(m_write);
        grown = true;
    }

#if CK_DEBUG
    byte* p = (byte*) &m_tasks[m_write];
    for (int i = 0; i < sizeof(T); ++i)
    {
        CK_ASSERT(*p == k_fillByte);
        ++p;
    }
#endif

    m_tasks[m_write] = task;
    m_write = nextWrite;
    return grown;
}

template <typename T>
bool TaskQueue<T>::consume(T& task) 
{
    MutexLock lock(m_mutex);
    /*
#if CK_DEBUG
    static int s_consumeThread = -1;
    if (s_consumeThread == -1)
    {
        s_consumeThread = Thread::getCurrentThreadId();
    }
    CK_ASSERT(Thread::getCurrentThreadId() == s_consumeThread);
#endif
*/

    if (m_read == m_write)
    {
        return false;  // queue is empty
    }

    task = m_tasks[m_read];

#if CK_DEBUG
    Mem::fill(m_tasks[m_read], k_fillByte);
#endif

    m_read = next(m_read);
    return true;
}

template <typename T>
bool TaskQueue<T>::isEmpty() const
{
    return m_read == m_write;
}

template <typename T>
int TaskQueue<T>::getCapacity() const
{
    return m_capacity;
}

///////////////////////////////////////

template <typename T>
int TaskQueue<T>::next(int i)
{
    ++i;
    if (i >= m_capacity)
    {
        i = 0;
    }

    return i;
}


}
