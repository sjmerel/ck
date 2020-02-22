#pragma once

#include "ck/core/platform.h"
#include "ck/config.h"
#include "ck/core/allocator.h"

namespace Cki
{


class FuncAllocator : public Allocator
{
public:
    FuncAllocator(CkAllocFunc, CkFreeFunc);

    virtual void* alloc(int bytes);
    virtual void free(void*);

private:
    CkAllocFunc m_alloc;
    CkFreeFunc m_free;
};



}
