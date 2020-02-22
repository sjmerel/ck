#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/core/debug.h"

namespace Cki
{



class MemoryFixup
{
public:
    MemoryFixup(void* buf, int size);

    void* getPointer();
    void advance(int bytes);
    bool isFailed() const;

    void fixup(uint8&)  {}
    void fixup(int8&)   {}
    void fixup(uint16&) {}
    void fixup(int16&)  {}
    void fixup(uint32&) {}
    void fixup(int32&)  {}
    void fixup(uint64&) {}
    void fixup(int64&)  {}
    void fixup(float&)  {}


    template <typename T>
    inline
    void fixup(T& value)
    {
        if (m_buf == m_pos)
        {
            m_pos += sizeof(T);
        }
        value.fixup(*this);
    }

    // for new serializable types, define these:
    // void T::fixup(MemoryFixup&);

private:
    byte* m_buf;
    byte* m_pos;
    int m_size;
    bool m_failed;

    MemoryFixup(const MemoryFixup&);
    MemoryFixup& operator=(const MemoryFixup&);
};



}

