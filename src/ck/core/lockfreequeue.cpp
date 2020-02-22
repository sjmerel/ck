#include "ck/core/lockfreequeue.h"
#include "ck/core/debug.h"
#include "ck/core/mem.h"
#include "ck/core/thread.h"

#if CK_PLATFORM_IOS || CK_PLATFORM_TVOS
#  include <libkern/OSAtomic.h>
#endif

namespace Cki
{


template <typename T>
LockFreeQueue<T>::LockFreeQueue(int size)
{
    int bufSize = size * sizeof(Node);
    m_buf = (byte*) Mem::alloc(bufSize);
    m_pool.init(sizeof(Node), m_buf, bufSize);

    void* mem = m_pool.alloc();
    m_first = new (mem) Node(T());
    m_divider = m_last = m_first;
}

template <typename T>
LockFreeQueue<T>::~LockFreeQueue()
{
    while (m_first)
    {
        // release the list
        Node* tmp = m_first;
        m_first = tmp->next;
        m_pool.free(tmp);
    }

    Mem::free(m_buf);
}

template <typename T>
bool LockFreeQueue<T>::produce(const T& t) 
{
    void* mem = m_pool.alloc();
    if (!mem)
    {
        return false;
    }
    ((Node*) m_last)->next = new (mem) Node(t);      // add the new item
    m_last = ((Node*) m_last)->next;     // publish it
    trim();
    return true;
}

template <typename T>
void LockFreeQueue<T>::trim()
{
#if CK_DEBUG
    static int s_produceThread = -1;
    if (s_produceThread == -1)
    {
        s_produceThread = Thread::getCurrentThreadId();
    }
    CK_ASSERT(Thread::getCurrentThreadId() == s_produceThread);
#endif

    while (m_first != m_divider) 
    {
        // trim unused nodes
        Node* tmp = m_first;
        m_first = m_first->next;
        m_pool.free(tmp);
    }
}

template <typename T>
bool LockFreeQueue<T>::consume(T& result) 
{
    if (((Node*) m_divider) != ((Node*) m_last)) 
    {
        // if queue is nonempty
        result = ((Node*) m_divider)->next->value; // copy it back

#if CK_PLATFORM_IOS || CK_PLATFORM_TVOS
        OSMemoryBarrier();
#endif
#if CK_PLATFORM_ANDROID
        __sync_synchronize();
#endif

        m_divider = ((Node*) m_divider)->next;   // publish that we took it
        return true;
    }
    return false;
}


}
