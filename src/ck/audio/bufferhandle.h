#pragma once

#include "ck/core/platform.h"
#include "ck/audio/sharedbuffer.h"

namespace Cki
{


class BufferHandle
{
public:
    BufferHandle(); // invalid
    BufferHandle(SharedBuffer*);

    ~BufferHandle();
    BufferHandle(const BufferHandle&);
    BufferHandle& operator=(const BufferHandle&);

    bool isValid() const   { return (m_buf != NULL); }
    void* get()   { return (m_buf ? m_buf->getMem() : NULL); }

    void release();

private:
    SharedBuffer* m_buf;
};


}
