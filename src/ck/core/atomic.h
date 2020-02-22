#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
#  include <libkern/OSAtomic.h>
#elif CK_PLATFORM_ANDROID
#  include <sys/atomics.h>
#elif CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  include <Windows.h>
#endif

namespace Cki
{


namespace Atomic
{
    inline bool compareAndSwap(int32 oldVal, int32 newVal, volatile int32* value)
    {
#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
        return OSAtomicCompareAndSwap32Barrier(oldVal, newVal, value);
#elif CK_PLATFORM_ANDROID
        return (__atomic_cmpxchg(oldVal, newVal, value) == oldVal);
#elif CK_PLATFORM_WIN || CK_PLATFORM_WP8
        return (InterlockedCompareExchange((LONG*)value, (LONG)newVal, (LONG)oldVal) == oldVal);
#else
#  error unsupported platform
#endif
    }
}


}
