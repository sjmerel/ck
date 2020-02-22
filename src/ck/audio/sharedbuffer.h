#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/core/list.h"

namespace Cki
{


class SharedBuffer : public List<SharedBuffer>::Node
{
public:
    SharedBuffer();
    SharedBuffer(void* mem);
    ~SharedBuffer();

    void init(void* mem);

    void* getMem()   { return m_mem; }
    void acquire();
    void release();
    int getRefs() const;

private:
    void* m_mem;
    int m_refs;

    SharedBuffer(const SharedBuffer&);
    SharedBuffer& operator=(const SharedBuffer&);
};


}
