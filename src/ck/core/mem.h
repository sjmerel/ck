#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/core/allocator.h"
#include <new>
#include <memory.h>
#include <string.h>

namespace Cki
{

namespace Mem
{
    void init(Allocator* alloc = NULL);
    void shutdown();


    ////////////////////////////////////////
    // allocate/free raw memory

    void* alloc(int bytes, int alignBytes = 4);
    void free(void* p);
    void verifyMem(void* p);
    int getSize(void* p);

    // for allocating arrays of primitive types only (ctors are not called)
    template <typename T>
    inline T* allocArray(int n, int alignBytes = 4)
    {
        return (T*) Mem::alloc(sizeof(T) * n, alignBytes);
    }


    ////////////////////////////////////////
    // fill memory

    inline void fill(void* p, byte value, int bytes)
    {
        memset(p, value, bytes);
    }

    template <typename T>
    inline void fill(T& t, byte value)
    {
        fill(&t, value, sizeof(T));
    }


    ////////////////////////////////////////
    // clear memory

    inline void clear(void* p, int bytes)
    {
        memset(p, 0, bytes);
    }

    template <typename T>
    inline void clear(T& t)
    {
        clear(&t, sizeof(T));
    }


    ////////////////////////////////////////

    // copy memory
    inline void copy(void* dst, const void* src, int bytes)
    {
        memcpy(dst, src, bytes);
    }

    // compare memory
    inline bool compare(const void* buf1, const void* buf2, int bytes)
    {
        return !memcmp(buf1, buf2, bytes);
    }



    ////////////////////////////////////////
    // swap

    inline uint8 swap(uint8 value) 
    { 
        return value; 
    }

    inline int8 swap(int8 value) 
    { 
        return value; 
    }

    inline uint16 swap(uint16 value)
    {
        return (uint16) ( ((value & 0x00ff) << 8) |
                          ((value & 0xff00) >> 8) );
    }

    inline int16 swap(int16 value)
    {
        return (int16) swap((uint16) value);
    }

    inline uint32 swap(uint32 value)
    {
        return ((value & 0x000000ff) << 24) |
               ((value & 0x0000ff00) << 8)  |
               ((value & 0x00ff0000) >> 8)  |
               ((value & 0xff000000) >> 24);
    }

    inline int32 swap(int32 value)
    {
        return (int32) swap((uint32) value);
    }

    inline uint64 swap(uint64 value)
    {
        return ((value & 0x00000000000000ffLL) << 56) |
               ((value & 0x000000000000ff00LL) << 40) |
               ((value & 0x0000000000ff0000LL) << 24) |
               ((value & 0x00000000ff000000LL) << 8)  |
               ((value & 0x000000ff00000000LL) >> 8)  |
               ((value & 0x0000ff0000000000LL) >> 24) |
               ((value & 0x00ff000000000000LL) >> 40) |
               ((value & 0xff00000000000000LL) >> 56);
    }
   
    inline int64 swap(int64 value)
    {
        return (int64) swap((uint64) value);
    }

    float swap(float value);
    double swap(double value);


    ////////////////////////////////////////
    // new/delete for objects without custom allocators
    // TODO: if used on objects with custom allocators, will call ctors/dtors multiple times!
    // TODO: specialize for native types; don't call ctors/dtors?

    template <typename T>
    inline T* New()
    {
        T* p = (T*) Mem::alloc(sizeof(T));

        // call constructor
        new(p) T;

        return p;
    }

    template <typename T, int N>
    inline T* NewArray()
    {
        T* p = (T*) Mem::alloc(sizeof(T) * N);

        // call constructors
        T* pp = p;
        for (int i = 0; i < N; ++i)
        {
            new(pp) T;
            ++pp;
        }

        return p;
    }

    template <typename T>
    inline void Delete(T* p)
    {
        // call destructor
        p->~T();
        Mem::free(p);
    }

    template <typename T, int N>
    inline void DeleteArray(T* p)
    {
        // TODO store size in buffer, instead of passing as N

        // call destructors
        T* pp = p;
        for (int i = 0; i < N; ++i)
        {
            pp->~T();
            ++pp;
        }

        Mem::free(p);
    }
}



}
