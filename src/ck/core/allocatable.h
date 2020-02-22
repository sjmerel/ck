#pragma once

#include "ck/core/platform.h"
#include <stddef.h>
#include <new>

namespace Cki
{


// Base class that defines new & delete operators that use our allocator

class Allocatable
{
public:
    void* operator new(size_t size) throw();
    void operator delete(void* p);
    void* operator new[](size_t size) throw();
    void operator delete[](void* p);

    // placement new/delete
    void* operator new(size_t size, void* p);
    void operator delete(void*, void* p);
};


}
