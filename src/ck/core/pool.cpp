#include "ck/core/pool.h"
#include "ck/core/debug.h"

namespace Cki
{


Pool::Pool() :
    m_elemSize(0),
    m_buf(NULL),
    m_bufEnd(NULL),
    m_free(NULL),
    m_freeCount(0)
{}

void Pool::init(int elemSize, void* buf, int bufSize)
{
    CK_ASSERT(elemSize % sizeof(void*) == 0); // must have pointer alignment
    CK_ASSERT(elemSize >= sizeof(void*)); // must be big enough for pointer
    CK_ASSERT(bufSize >= elemSize);

    m_elemSize = elemSize;
    m_buf = (byte*) buf;

    int numElems = bufSize / m_elemSize;
    m_bufEnd = m_buf + numElems * m_elemSize;

    // each slot contains next item in free list
    byte* p = m_buf;
    while (p < m_bufEnd - m_elemSize)
    {
        *((byte**) p) = p + m_elemSize;
        p += m_elemSize;
    }

    // last one
    *((byte**) (m_bufEnd - m_elemSize)) = 0;

    m_free = m_buf;
    m_freeCount = numElems;
}

bool Pool::isInited() const
{
    return m_buf != NULL;
}

void* Pool::alloc()
{
    CK_ASSERT(m_buf);
    if (m_free)
    {
        byte* p = m_free;
        m_free = *((byte**) m_free);
        --m_freeCount;
        return p;
    }
    else
    {
        return NULL;
    }
}

void Pool::free(void* mem)
{
    CK_ASSERT(m_buf);
#if CK_DEBUG
    // check if already in free list
    byte* q = m_free;
    while (q)
    {
        CK_ASSERT(mem != q);
        q = *((byte**)q);
    }
#endif
    byte* p = (byte*) mem;
    CK_ASSERT(p >= m_buf && p < m_bufEnd);
    CK_ASSERT((p - m_buf) % m_elemSize == 0);
    *((byte**) p) = m_free;
    ++m_freeCount;
    m_free = p;
}

bool Pool::contains(const void* mem) const
{
    CK_ASSERT(m_buf);
    return (mem >= m_buf && mem < m_bufEnd);
}

int Pool::getFreeCount() const
{
    return m_freeCount;
}


}
