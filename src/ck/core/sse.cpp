#include "ck/core/sse.h"

#if CK_X86_SSE

#if CK_PLATFORM_WIN || CK_PLATFORM_WP8
#  include <intrin.h>
#elif CK_PLATFORM_LINUX
#  include <cpuid.h>
#endif

namespace Cki
{


namespace Sse
{
    bool isSupported()
    {
#if CK_PLATFORM_ANDROID || CK_PLATFORM_OSX
        return true;
#elif CK_PLATFORM_WIN || CK_PLATFORM_WP8
        int info[4];
        __cpuid(info, 0);
        int nIds = info[0];
        if (nIds > 0)
        {
            __cpuid(info, 1);
            return (info[3] & ((int) 1 << 26)) != 0;

            //        HW_SSE    = (info[3] & ((int)1 << 25)) != 0;
            //        HW_SSE2   = (info[3] & ((int)1 << 26)) != 0;
            //        HW_SSE3   = (info[2] & ((int)1 <<  0)) != 0;
            //        HW_SSSE3  = (info[2] & ((int)1 <<  9)) != 0;
            //        HW_SSE41  = (info[2] & ((int)1 << 19)) != 0;
            //        HW_SSE42  = (info[2] & ((int)1 << 20)) != 0;
        }
        return false;
#elif CK_PLATFORM_LINUX
        if (__get_cpuid_max(0, NULL) > 0)
        {
            uint eax, ebx, ecx, edx;
            if (__get_cpuid(1, &eax, &ebx, &ecx, &edx) != 0)
            {
                return (edx & ((int) 1 << 26)) != 0;
            }
        }
        return false;
#else
        CK_FAIL("unknown SSE platform");
        return false;
#endif
    }
}


}

#endif
