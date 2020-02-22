#pragma once

#include "ck/core/platform.h"

namespace Cki
{


class Allocator
{
public:
    virtual ~Allocator() {}

    virtual void* alloc(int bytes) = 0;
    virtual void free(void*) = 0;
};


}
