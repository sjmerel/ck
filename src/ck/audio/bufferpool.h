#pragma once

#include "ck/core/platform.h"
#include "ck/core/allocatable.h"
#include "ck/core/list.h"
#include "ck/audio/sharedbuffer.h"
#include "ck/audio/bufferhandle.h"

namespace Cki
{


class BufferPool : public Allocatable
{
public:
    BufferPool(int bufSize); // number of values per buffer
    ~BufferPool();

    void add(SharedBuffer&);
    void remove(SharedBuffer&);

    BufferHandle acquire();
    int getBufferSize() const;

#if CK_DEBUG
    void verify();
#endif

private:
    enum { k_numBufs = 2 };
    SharedBuffer m_bufs[k_numBufs];
    int m_bufSize;
    List<SharedBuffer> m_bufList;

    BufferPool(const BufferPool&);
    BufferPool& operator=(const BufferPool&);
};

////////////////////////////////////////

inline
int BufferPool::getBufferSize() const
{
    return m_bufSize;
}

}
