#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

namespace Cki
{


// Pool of equal-sized memory blocks

class Pool
{
public:
    Pool();

    void init(int elemSize, void* buf, int bufSize);
    bool isInited() const;

    void* alloc(); // returns NULL if no free blocks
    void free(void*);
    bool contains(const void*) const;

    int getFreeCount() const;

private:
    int m_elemSize;
    byte* m_buf;
    byte* m_bufEnd;
    byte* m_free;
    int m_freeCount;
};


}
