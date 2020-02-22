#include "ck/core/allocatable.h"
#include "ck/core/mem.h"

namespace Cki
{


void* Allocatable::operator new(size_t size) throw()
{
    return Mem::alloc((int) size);
}

void Allocatable::operator delete(void* p)
{
    Mem::free(p);
}

void* Allocatable::operator new[](size_t size) throw()
{
    return Mem::alloc((int) size);
}

void Allocatable::operator delete[](void* p)
{
    Mem::free(p);
} 

void* Allocatable::operator new(size_t size, void* p)
{
    return ::operator new(size, p);
}

void Allocatable::operator delete(void*, void*)
{
}


}
