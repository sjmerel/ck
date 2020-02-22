#include "ck/audio/bufferpool.h"
#include "ck/core/debug.h"
#include "ck/core/mem.h"

namespace Cki
{


BufferPool::BufferPool(int bufSize) :
    m_bufSize(bufSize)
{
    for (int i = 0; i < k_numBufs; ++i)
    {
        // 64-byte alignment (16 words) for faster ARM NEON accesses
        void* buf = Mem::alloc(bufSize * sizeof(int32), 64);
        m_bufs[i].init(buf);
        m_bufList.addFirst(&m_bufs[i]);
    }
}

BufferPool::~BufferPool()
{
    for (int i = 0; i < k_numBufs; ++i)
    {
        CK_ASSERT(m_bufs[i].getRefs() == 0);
        Mem::free(m_bufs[i].getMem());
    }
}

void BufferPool::add(SharedBuffer& buf)
{
    m_bufList.addFirst(&buf);
}

void BufferPool::remove(SharedBuffer& buf)
{
    m_bufList.remove(&buf);
}

BufferHandle BufferPool::acquire()
{
#if CK_DEBUG
    verify();
#endif

    SharedBuffer* buf = m_bufList.getFirst();
    while (buf)
    {
        if (buf->getRefs() == 0)
        {
            return BufferHandle(buf);
        }
        buf = ((List<SharedBuffer>::Node*) buf)->getNext();
    }

    CK_FAIL("out of buffers");
    return BufferHandle();
}

////////////////////////////////////////

#if CK_DEBUG
void BufferPool::verify()
{
    for (int i = 0; i < k_numBufs; ++i)
    {
        void* buf = m_bufs[i].getMem();
        Mem::verifyMem(buf);
    }
}
#endif

}
