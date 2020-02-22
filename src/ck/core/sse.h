#pragma once

#include "ck/core/platform.h"

#if CK_X86_SSE

#include <emmintrin.h>

namespace Cki
{


namespace Sse
{

#ifdef _MSC_VER
#  define CK_CAST_M128_M128I(x) _mm_castps_si128(x)
#  define CK_CAST_M128I_M128(x) _mm_castsi128_ps(x)
#else
#  define CK_CAST_M128_M128I(x) x
#  define CK_CAST_M128I_M128(x) x
#endif

    inline float getElement(__m128 x, int i)
    {
#ifdef _MSC_VER
        return x.m128_f32[i];
#else
        return x[i];
#endif
    }

    inline int getElement(__m128i x, int i)
    {
#ifdef _MSC_VER
        return x.m128i_i32[i];
#else
        return ((int*)&x)[i];
#endif
    }

    bool isSupported();

}


}

#endif // CK_X86_SSE
