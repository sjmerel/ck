#include "ck/audio/sharedbuffer.h"
#include "ck/core/debug.h"

// template instantiation
#include "ck/core/list.cpp"

namespace Cki
{

SharedBuffer::SharedBuffer() :
    m_mem(NULL),
    m_refs(0)
{}

SharedBuffer::SharedBuffer(void* mem) :
    m_mem(mem),
    m_refs(0)
{}

SharedBuffer::~SharedBuffer()
{
    CK_ASSERT(!m_refs);
}

void SharedBuffer::init(void* mem)
{
    CK_ASSERT(!m_mem);
    CK_ASSERT(!m_refs);
    m_mem = mem;
    m_refs = 0;
}

void SharedBuffer::acquire()
{
    CK_ASSERT(m_refs >= 0);
    ++m_refs;
}

void SharedBuffer::release()
{
    --m_refs;
    CK_ASSERT(m_refs >= 0);
}

int SharedBuffer::getRefs() const
{
    return m_refs;
}


template class List<SharedBuffer>;

}
