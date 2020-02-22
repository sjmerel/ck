#include "ck/core/list.h"
#include "ck/core/debug.h"

namespace Cki
{


template <typename T, int N>
List<T,N>::List() :
    m_first(NULL),
    m_last(NULL),
    m_size(0)
{
}

template <typename T, int N>
List<T,N>::~List()
{
    clear();
}

template <typename T, int N>
T* List<T,N>::getFirst()
{
    return (T*) m_first;
}

template <typename T, int N>
T* List<T,N>::getLast()
{
    return (T*) m_last;
}

template <typename T, int N>
void List<T,N>::addFirst(T* t)
{
    Node* node = (Node*) t;
    if (m_first)
    {
        addBefore(t, (T*) m_first);
    }
    else
    {
        CK_ASSERT(!m_last);
        m_first = node;
        m_last = node;
        ++m_size;
    }
}

template <typename T, int N>
void List<T,N>::addLast(T* t)
{
    Node* node = (Node*) t;
    if (m_last)
    {
        addAfter(t, (T*) m_last);
    }
    else
    {
        CK_ASSERT(!m_first);
        m_first = node;
        m_last = node;
        ++m_size;
    }
}

template <typename T, int N>
void List<T,N>::addBefore(T* t, T* pos)
{
#if CK_DEBUG
    CK_ASSERT(contains(pos));
#endif
    if (t != pos)
    {
        remove(t);

        Node* node = (Node*) t;
        Node* posNode = (Node*) pos;
        node->m_next = posNode;
        node->m_prev = posNode->m_prev;
        posNode->m_prev = node;
        if (node->m_prev)
        {
            node->m_prev->m_next = node;
        }
        else
        {
            CK_ASSERT(m_first == posNode);
            m_first = node;
        }

        CK_ASSERT(m_size >= 0);
        ++m_size;
    }
}

template <typename T, int N>
void List<T,N>::addAfter(T* t, T* pos)
{
#if CK_DEBUG
    CK_ASSERT(contains(pos));
#endif

    if (t != pos)
    {
        remove(t);

        Node* node = (Node*) t;
        Node* posNode = (Node*) pos;
        node->m_prev = posNode;
        node->m_next = posNode->m_next;
        posNode->m_next = node;
        if (node->m_next)
        {
            node->m_next->m_prev = node;
        }
        else
        {
            CK_ASSERT(m_last == posNode);
            m_last = node;
        }

        CK_ASSERT(m_size >= 0);
        ++m_size;
    }
}

template <typename T, int N>
bool List<T,N>::contains(T* t) const
{
    Node* p = m_first;
    while (p)
    {
        if ((T*) p == t)
        {
            return true;
        }
        p = p->m_next;
    }
    return false;
}

template <typename T, int N>
int List<T,N>::getSize() const
{
    return m_size;
}

template <typename T, int N>
bool List<T,N>::isEmpty() const
{
    return m_size == 0;
}

template <typename T, int N>
void List<T,N>::remove(T* t)
{
    if (contains(t))
    {
        Node* node = (Node*) t;
        if (m_first == node)
        {
            CK_ASSERT(!node->m_prev);
            m_first = node->m_next;
        }
        if (m_last == node)
        {
            CK_ASSERT(!node->m_next);
            m_last = node->m_prev;
        }

        if (node->m_prev)
        {
            node->m_prev->m_next = node->m_next;
        }
        if (node->m_next)
        {
            node->m_next->m_prev = node->m_prev;
        }

        node->m_prev = NULL;
        node->m_next = NULL;

        --m_size;
        CK_ASSERT(m_size >= 0);
    }
}

template <typename T, int N>
void List<T,N>::clear()
{
    Node* p = m_first;
    while (p)
    {
        Node* q = p;
        p = p->m_next;
        q->m_prev = NULL;
        q->m_next = NULL;
    }

    m_first = NULL;
    m_last = NULL;
    m_size = 0;
}

#if CK_DEBUG
template <typename T, int N>
void List<T,N>::verify()
{
    CK_ASSERT(m_size >= 0);
    CK_ASSERT((m_first && m_last) || (!m_first && !m_last));
    CK_ASSERT((m_size && m_first) || (!m_size && !m_first));
    CK_ASSERT(!m_first || (m_first->m_prev == NULL));
    CK_ASSERT(!m_last || (m_last->m_next == NULL));
    Node* p = m_first;
    while (p)
    {
        CK_ASSERT(p->m_prev == NULL || p->m_prev->m_next == p);
        CK_ASSERT(p->m_next == NULL || p->m_next->m_prev == p);
        p = p->m_next;
    }
}
#endif


}
