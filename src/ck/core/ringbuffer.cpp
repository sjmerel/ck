#include "ck/core/ringbuffer.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/mem.h"
#include "ck/core/mutexlock.h"
#include <memory.h>

namespace Cki
{

// Writes and reads can happen on different threads; at worst we will read
// or write less than possible.  The one value currently protected by a mutex
// is m_stored, which is updated by both reads and writes.

// (Could instead keep separate counts of bytes read & written, but we could
// potentially overflow; is there some lock-free way to do this?)

RingBuffer::RingBuffer() :
    m_buf(NULL),
    m_bufSize(0),
    m_readPos(0),
    m_writePos(0),
    m_stored(0),
    m_reading(false),
    m_writing(false)
{
}

RingBuffer::RingBuffer(int bytes) :
    m_buf(NULL),
    m_bufSize(0),
    m_readPos(0),
    m_writePos(0),
    m_stored(0),
    m_reading(false),
    m_writing(false)
{
    init(bytes);
}

RingBuffer::~RingBuffer()
{
    CK_ASSERT(!m_reading && !m_writing);
    Mem::free(m_buf);
}

void RingBuffer::init(int bytes)
{
    CK_ASSERT(!m_buf);
    m_buf = (byte*) Mem::alloc(bytes);
    if (m_buf)
    {
        m_bufSize = bytes;
    }
}

int RingBuffer::read(void* buf, int bytes)
{
    const void* buf1;
    const void* buf2;
    int bytes1;
    int bytes2;
    beginRead(bytes, buf1, bytes1, buf2, bytes2);
    if (buf1 && buf)
    {
        byte* p = (byte*) buf;
        Mem::copy(p, buf1, bytes1);
        if (buf2)
        {
            Mem::copy(p + bytes1, buf2, bytes2);
        }
    }
    endRead(bytes1 + bytes2);
    return bytes1 + bytes2;
}

int RingBuffer::write(const void* buf, int bytes)
{
    void* buf1;
    void* buf2;
    int bytes1; 
    int bytes2;
    beginWrite(bytes, buf1, bytes1, buf2, bytes2);
    if (buf1)
    {
        const byte* p = (const byte*) buf;
        Mem::copy(buf1, p, bytes1);
        if (buf2)
        {
            Mem::copy(buf2, p + bytes1, bytes2);
        }
    }
    endWrite(bytes1 + bytes2);
    return bytes1 + bytes2;
}

int RingBuffer::consume(int bytes)
{
    return read(NULL, bytes);
}

void RingBuffer::beginRead(int bytes, const void*& buf1, int& bytes1, const void*&buf2, int& bytes2)
{
    CK_ASSERT(bytes >= 0);
    CK_ASSERT(!m_reading);
    m_reading = true;

    // cache write pos, since another thread might be writing
    int writePos = m_writePos;

    if (m_stored == 0)
    {
        buf1 = buf2 = NULL;
        bytes1 = bytes2 = 0;
    }
    else
    {
        if (writePos > m_readPos)
        {
            buf1 = m_buf + m_readPos;
            bytes1 = Math::min(writePos - m_readPos, bytes);
            buf2 = NULL;
            bytes2 = 0;
        }
        else
        {
            buf1 = m_buf + m_readPos;
            bytes1 = Math::min(m_bufSize - m_readPos, bytes);
            if (bytes > bytes1)
            {
                buf2 = m_buf;
                bytes2 = Math::min(writePos, bytes - bytes1);
            }
            else
            {
                buf2 = NULL;
                bytes2 = 0;
            }
        }
    }
}

void RingBuffer::endRead(int bytes)
{
    CK_ASSERT(m_reading);
    m_reading = false;

    m_readPos += bytes;
    if (m_readPos >= m_bufSize)
    {
        m_readPos -= m_bufSize;
    }
    CK_ASSERT(m_readPos >= 0 && m_readPos <= m_bufSize);

    {
        MutexLock lock(m_storedMutex);
        m_stored -= bytes;
    }
    CK_ASSERT(m_stored >= 0 && m_stored <= m_bufSize);
}

void RingBuffer::beginWrite(int bytes, void*& buf1, int& bytes1, void*&buf2, int& bytes2)
{
    CK_ASSERT(bytes >= 0);
    CK_ASSERT(!m_writing);
    m_writing = true;

    // cache read pos, since another thread might be reading
    int readPos = m_readPos;

    if (m_stored == m_bufSize)
    {
        buf1 = buf2 = NULL;
        bytes1 = bytes2 = 0;
    }
    else
    {
        if (readPos > m_writePos)
        {
            buf1 = m_buf + m_writePos;
            bytes1 = Math::min(readPos - m_writePos, bytes);
            buf2 = NULL;
            bytes2 = 0;
        }
        else
        {
            buf1 = m_buf + m_writePos;
            bytes1 = Math::min(m_bufSize - m_writePos, bytes);
            if (bytes > bytes1)
            {
                bytes2 = Math::min(readPos, bytes - bytes1);
                buf2 = bytes2 ? m_buf : NULL;
            }
            else
            {
                buf2 = NULL;
                bytes2 = 0;
            }
        }
    }
}

void RingBuffer::endWrite(int bytes)
{
    CK_ASSERT(m_writing);
    m_writing = false;

    m_writePos += bytes;
    if (m_writePos >= m_bufSize)
    {
        m_writePos -= m_bufSize;
    }
    CK_ASSERT(m_writePos >= 0 && m_writePos <= m_bufSize);

    {
        MutexLock lock(m_storedMutex);
        m_stored += bytes;
    }
    CK_ASSERT(m_stored >= 0 && m_stored <= m_bufSize);
}

void RingBuffer::resetRead()
{
    CK_ASSERT(!m_reading);
    m_readPos = m_writePos;

    {
        MutexLock lock(m_storedMutex);
        m_stored = 0;
    }
}

void RingBuffer::resetWrite()
{
    CK_ASSERT(!m_writing);
    m_writePos = m_readPos;

    {
        MutexLock lock(m_storedMutex);
        m_stored = 0;
    }
}



}
