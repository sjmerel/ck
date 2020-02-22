#include "ck/core/lockingqueue.h"
#include "ck/core/debug.h"
#include "ck/core/mem.h"
#include "ck/core/mutexlock.h"

namespace Cki
{


template <typename T>
LockingQueue<T>::LockingQueue(int size)
{
    int bufSize = size * sizeof(Node);
    m_buf = (byte*) Mem::alloc(bufSize);
    m_pool.init(sizeof(Node), m_buf, bufSize);

    m_first = NULL;
    m_last = NULL;
}

template <typename T>
LockingQueue<T>::~LockingQueue()
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
bool LockingQueue<T>::produce(const T& t) 
{
    MutexLock lock(m_mutex);

    void* mem = m_pool.alloc();
    if (!mem)
    {
        return false;
    }

    Node* node = new (mem) Node(t);
    if (m_first)
    {
        CK_ASSERT(m_last);
        CK_ASSERT(m_first->prev == NULL);
        CK_ASSERT(m_last->next == NULL);
        node->next = m_first;
        m_first->prev = node;
        m_first = node;
    }
    else
    {
        CK_ASSERT(!m_last);
        m_first = node;
        m_last = node;
    }

    return true;
}

template <typename T>
bool LockingQueue<T>::consume(T& result) 
{
    MutexLock lock(m_mutex);

    if (m_last)
    {
        // queue is nonempty
        CK_ASSERT(m_first);
        CK_ASSERT(m_first->prev == NULL);
        CK_ASSERT(m_last->next == NULL);

        result = m_last->value;

        Node* n = m_last->prev;
        m_pool.free(m_last);
        if (n)
        {
            CK_ASSERT(m_last != m_first);
            m_last = n;
            m_last->next = NULL;
        }
        else
        {
            CK_ASSERT(m_last == m_first);
            m_last = NULL;
            m_first = NULL;
        }

        return true;
    }
    else
    {
        CK_ASSERT(!m_first);
        return false;
    }
}


}

