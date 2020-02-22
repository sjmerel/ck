#include "ck/core/mem.h"
#include "ck/core/math.h"
#include "ck/core/debug.h"
#include "ck/core/thread.h"
#include "ck/core/logger.h"
#include <memory.h>

#define CK_MEM_DEBUG CK_DEBUG

namespace Cki
{

// hooks for replacing standard memory functions
extern "C"
{
    void* CK_malloc(size_t size)
    {
        return Mem::alloc((int) size);
    }

    void* CK_calloc(size_t count, size_t size)
    {
        int bytes = (int)(size*count);
        void* p = Mem::alloc(bytes);
        if (p)
        {
            Mem::clear(p, bytes);
        }
        return p;
    }

    void* CK_realloc(void* p, size_t size)
    {
        void* pNew = NULL;
        if (size > 0)
        {
            pNew = Mem::alloc((int) size);
        }

        if (pNew && p)
        {
            int oldSize = Mem::getSize(p);
            Mem::copy(pNew, p, Math::min((int) size, oldSize));
        }

        Mem::free(p);
        return pNew;
    }

    void CK_free(void* p)
    {
        Mem::free(p);
    }
}


#if CK_MEM_DEBUG
static const uint32 k_guardValue = 0xfeedface;
#endif

class SystemAllocator : public Allocator
{
public:
    virtual void* alloc(int bytes)
    {
        return new uint8[bytes];
    }

    virtual void free(void* p)
    {
        delete[] (char*) p;
    }
};

namespace
{
    Allocator* g_allocator = NULL;
    SystemAllocator g_systemAllocator;

#if CK_MEM_DEBUG
    int g_allocedBytes = 0;
#endif
    int g_initThreadId = 0; // ID of thread that called init()
}


namespace Mem
{
    // in release mode:
    //   [ alignment padding (0+ words) ] [ padding words (1) ] [ alloc size (1) ] [ data ... ]
    // in debug mode:
    //   [ alignment padding (0+ words) ] [ padding words (1) ] [ alloc size (1) ] [ guard (1) ] [ data ... ] [ guard (1) ]
    void* alloc(int bytes, int alignBytes)
    {
//        CK_ASSERT(Thread::getCurrentThreadId() == g_initThreadId);
        CK_ASSERT(bytes >= 0);

        CK_ASSERT(alignBytes % 4 == 0);
        alignBytes = Math::max(alignBytes, 4);
        int alignWords = alignBytes / 4;

        int prefixWords = 2; // padding size and alloc size
#if CK_MEM_DEBUG 
        prefixWords += 1; // start guard 
#endif

        int sizeWords = (alignWords - 1) + prefixWords; // possible alignment padding
#if CK_MEM_DEBUG 
        sizeWords += 1; // end guard
#endif

        uint32* buf = (uint32*) g_allocator->alloc(bytes + sizeWords * 4);
        if (!buf)
        {
            CK_LOG_ERROR("out of memory!");
            return buf;
        }

        CK_ASSERT((((size_t) buf) & 3) == 0); // must be 4-byte aligned

        uint32* user = buf + prefixWords;
        int rem = ((size_t) user) % alignBytes;
        int padWords = 0;
        if (rem != 0)
        {
            padWords = (alignBytes - rem) / 4;
        }
        user += padWords;

#if CK_MEM_DEBUG
        *(user - 1) = k_guardValue; // start guard
        Mem::copy((byte*) user + bytes, &k_guardValue, sizeof(uint32)); // end guard

        *(user - 2) = bytes;
        g_allocedBytes += bytes;

        *(user - 3) = padWords;
#else
        *(user - 1) = bytes;
        *(user - 2) = padWords;
#endif

        CK_ASSERT(((size_t) user) % alignBytes == 0);

        return user;
    }

    void free(void* p)
    {
//        CK_ASSERT(Thread::getCurrentThreadId() == g_initThreadId);
        if (!p)
        {
            return;
        }
        else
        {
            uint32* user = (uint32*) p;
#if CK_MEM_DEBUG
            uint32 bytes = *(user - 2);
            g_allocedBytes -= bytes;
            CK_ASSERT(g_allocedBytes >= 0);

            // check start guard
            uint32 guard = *(user - 1);
            CK_ASSERT(guard == k_guardValue);

            // check end guard
            Mem::copy(&guard, (byte*) user + bytes, sizeof(uint32));
            CK_ASSERT(guard == k_guardValue);

            // zero out memory and guards, to help catch stale pointers
            fill(user - 1, 0xab, bytes + 2*sizeof(uint32));

            uint32 padWords = *(user - 3);
            g_allocator->free(user - 3 - padWords);
#else
            uint32 padWords = *(user - 2);
            g_allocator->free(user - 2 - padWords);
#endif
        }
    }

    void verifyMem(void* p)
    {
#if CK_MEM_DEBUG
        uint32* user = (uint32*) p;
        uint32 bytes = *(user - 2);

        // check start guard
        uint32 guard = *(user - 1);
        CK_ASSERT(guard == k_guardValue);

        // check end guard
        Mem::copy(&guard, (byte*) user + bytes, sizeof(uint32));
        CK_ASSERT(guard == k_guardValue);
#endif
    }

    int getSize(void* p)
    {
        uint32* user = (uint32*) p;
#if CK_MEM_DEBUG
        return *(user-2);
#else
        return *(user-1);
#endif
    }

    float swap(float value)
    {
        CK_STATIC_ASSERT(sizeof(float)==sizeof(uint32));
        uint32* p = (uint32*) &value;
        *p = swap(*p);
        return value;
    }

    double swap(double value)
    {
        CK_STATIC_ASSERT(sizeof(double)==sizeof(uint64));
        uint64* p = (uint64*) &value;
        *p = swap(*p);
        return value;
    }


    void init(Allocator* alloc)
    {
        CK_ASSERT(!g_allocator);
        g_initThreadId = Thread::getCurrentThreadId();
        if (alloc)
        {
            g_allocator = alloc;
        }
        else
        {
            g_allocator = &g_systemAllocator;
        }
    }

    void shutdown()
    {
        CK_ASSERT(g_allocator);
        g_allocator = NULL;
#if CK_MEM_DEBUG
        if (g_allocedBytes != 0)
        {
            CK_LOG_ERROR("leaked %d bytes", g_allocedBytes);
        }
#endif
    }
}


}
