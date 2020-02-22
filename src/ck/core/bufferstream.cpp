#include "ck/core/bufferstream.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/mem.h"

namespace Cki
{


BufferStream::BufferStream(void* buf, int bufSize, int size) :
    m_buf((byte*) buf),
    m_bufSize(bufSize),
    m_size(size),
    m_pos(0)
{
    CK_ASSERT(buf);
    CK_ASSERT(bufSize >= 0);
    CK_ASSERT(size >= 0);
}

int BufferStream::read(void* buf, int bytes)
{
    CK_ASSERT(buf);
    CK_ASSERT(bytes >= 0);
    int bytesToEnd = Math::max(m_size - m_pos, 0);
    int bytesToRead = Math::min(bytes, bytesToEnd);
    if (bytesToRead > 0)
    {
        Mem::copy(buf, m_buf + m_pos, bytesToRead);
        m_pos += bytesToRead;
        CK_ASSERT(m_pos >= 0 && m_pos <= m_size);
        CK_ASSERT(m_pos <= m_bufSize);
    }
    return bytesToRead;
}

int BufferStream::write(const void* buf, int bytes)
{
    CK_ASSERT(buf || bytes == 0);
    CK_ASSERT(bytes >= 0);
    int bytesToEnd = Math::max(m_bufSize - m_pos, 0);
    int bytesToWrite = Math::min(bytes, bytesToEnd);
    if (m_pos + bytesToWrite > m_size)
    {
        m_size = m_pos + bytesToWrite;
    }
    if (bytesToWrite > 0)
    {
        Mem::copy(m_buf + m_pos, buf, bytesToWrite);
        m_pos += bytesToWrite;
    }
    CK_ASSERT(m_pos >= 0 && m_pos <= m_bufSize);
    return bytesToWrite;
}

int BufferStream::getSize() const
{
    return m_size;
}

int BufferStream::getPos() const
{
    return m_pos;
}

void BufferStream::setPos(int pos)
{
    CK_ASSERT(pos >= 0);
    m_pos = pos;
}

int BufferStream::getCapacity() const
{
    return m_bufSize;
}



}
