#include "ck/core/memorystream.h"
#include "ck/core/debug.h"
#include "ck/core/mem.h"
#include "ck/core/math.h"
#include <memory.h>

namespace Cki
{


MemoryStream::MemoryStream() :
    m_buf(NULL),
    m_bufSize(0),
    m_size(0),
    m_pos(0)
{
}

MemoryStream::MemoryStream(int capacity) :
    m_buf(NULL),
    m_bufSize(0),
    m_size(0),
    m_pos(0)
{
    reserve(capacity);
}

MemoryStream::~MemoryStream()
{
    close();
}

int MemoryStream::read(void* buf, int bytes)
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
    }
    return bytesToRead;
}

int MemoryStream::write(const void* buf, int bytes)
{
    CK_ASSERT(buf || bytes == 0);
    CK_ASSERT(bytes >= 0);
    reserve(m_pos + bytes); // TODO grow more agressively?
    if (bytes > 0)
    {
        Mem::copy(m_buf + m_pos, buf, bytes);
        m_pos += bytes;
    }
    if (m_pos > m_size)
    {
        m_size = m_pos;
    }
    CK_ASSERT(m_pos >= 0 && m_pos <= m_bufSize);
    CK_ASSERT(m_size >= 0 && m_size <= m_bufSize);
    return bytes;
}

int MemoryStream::getSize() const
{
    return m_size;
}

int MemoryStream::getPos() const
{
    return m_pos;
}

void MemoryStream::setPos(int pos)
{
    CK_ASSERT(pos >= 0);
    m_pos = pos;
}

void MemoryStream::close()
{
    Mem::free(m_buf);
    m_buf = NULL;
}

void* MemoryStream::getBuffer()
{
    return m_buf;
}

int MemoryStream::getCapacity() const
{
    return m_bufSize;
}

void MemoryStream::reserve(int capacity)
{
    if (capacity > m_bufSize)
    {
        void* newBuf = Mem::alloc(capacity);
        if (m_buf)
        {
            Mem::copy(newBuf, m_buf, m_bufSize);
        }
        Mem::free(m_buf);
        m_buf = (byte*) newBuf;
        m_bufSize = capacity;
    }
}

void MemoryStream::compact()
{
    if (m_bufSize > m_size)
    {
        if (m_size > 0)
        {
            void* newBuf = Mem::alloc(m_size);
            Mem::copy(newBuf, m_buf, m_size);
            Mem::free(m_buf);
            m_buf = (byte*) newBuf;
            m_bufSize = m_size;
        }
        else
        {
            Mem::free(m_buf);
            m_buf = NULL;
            m_bufSize = 0;
        }
    }
}

}
