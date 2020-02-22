#include "ck/core/funcallocator.h"

namespace Cki
{


FuncAllocator::FuncAllocator(CkAllocFunc allocFunc, CkFreeFunc freeFunc) :
    m_alloc(allocFunc),
    m_free(freeFunc)
{}

void* FuncAllocator::alloc(int bytes)
{
    return m_alloc(bytes);
}

void FuncAllocator::free(void* buf)
{
    m_free(buf);
}


}
