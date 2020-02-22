#include "ck/core/memoryfixup.h"

namespace Cki
{


MemoryFixup::MemoryFixup(void* buf, int size) :
    m_buf((byte*) buf),
    m_pos(m_buf),
    m_size(size),
    m_failed(false)
{}

void* MemoryFixup::getPointer()
{
    return m_pos;
}

void MemoryFixup::advance(int bytes)
{
    m_pos += bytes;
//    CK_ASSERT(m_pos <= m_buf + m_size);
    if (m_pos > m_buf + m_size)
    {
        m_failed = true;
        m_pos = m_buf + m_size;
    }
}

bool MemoryFixup::isFailed() const
{
    return m_failed;
}

}
