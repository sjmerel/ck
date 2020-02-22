#include "ck/audio/bufferhandle.h"
#include "ck/audio/sharedbuffer.h"
#include "ck/core/debug.h"

namespace Cki
{


BufferHandle::BufferHandle() :
    m_buf(NULL)
{}

BufferHandle::BufferHandle(SharedBuffer* buf) :
    m_buf(buf)
{
    CK_ASSERT(m_buf->getRefs() == 0);
    m_buf->acquire();
}

BufferHandle::~BufferHandle()
{
    if (m_buf)
    {
        m_buf->release();
    }
}

BufferHandle::BufferHandle(const BufferHandle& other) :
    m_buf(other.m_buf)
{
    if (m_buf)
    {
        m_buf->acquire();
    }
}

BufferHandle& BufferHandle::operator=(const BufferHandle& other)
{
    if (this != &other)
    {
        if (m_buf)
        {
            m_buf->release();
        }
        m_buf = other.m_buf;
        if (m_buf)
        {
            m_buf->acquire();
        }
    }
    return *this;
}

void BufferHandle::release()
{
    if (m_buf)
    {
        m_buf->release();
        m_buf = NULL;
    }
}



}
