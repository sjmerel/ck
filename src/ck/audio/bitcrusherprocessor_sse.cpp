#include "ck/audio/bitcrusherprocessor.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/sse.h"

#if CK_X86_SSE
#  include <emmintrin.h>
#endif

namespace Cki
{


#if CK_X86_SSE
void BitCrusherProcessor::process_sse(int* inBuf, int* outBuf, int frames)
{
    process_default(inBuf, outBuf, frames);
}

void BitCrusherProcessor::process_sse(float* inBuf, float* outBuf, int frames)
{
    // XXX neon implementation is slow for hold = 0, so using default instead;
    // process() is overridden in bitcrusherprocessor.cpp

    int holdFrames = (int) (getSampleRate() * m_holdMs * 0.001f);
    int shiftBits = 24 - m_bits;
    uint mask = (0xffffffff >> shiftBits) << shiftBits;

    if (holdFrames == 0 && shiftBits == 0)
    {
        return;
    }

    // copy member vars to locals
    int holdFramesLeft = m_holdFramesLeft;

    float* p = inBuf;
    float* pEnd = p + frames*2;

    const __m128 zero = _mm_setzero_ps();
    const __m128 left = _mm_set1_ps((float) (1 << 24));
    const __m128 right = _mm_set1_ps(1.0f / (1 << 24));

    const __m128i maskv = _mm_set1_epi32(mask);
    __m128 holdValue = _mm_loadh_pi(zero, (const __m64*) &m_holdValue[0].floatValue);

    while (p < pEnd)
    {
        if (holdFramesLeft > 0)
        {
            _mm_storeh_pi((__m64*) p, holdValue);
            p += 2;
            --holdFramesLeft;
        }
        else
        {
            holdValue = _mm_loadh_pi(zero, (const __m64*) p);
            __m128 tmp = _mm_mul_ps(holdValue, left);
            __m128i intValue = _mm_cvtps_epi32(tmp);
            intValue = CK_CAST_M128_M128I(_mm_and_ps(CK_CAST_M128I_M128(intValue), CK_CAST_M128I_M128(maskv)));
            tmp = _mm_cvtepi32_ps(intValue);
            holdValue = _mm_mul_ps(tmp, right);
            _mm_storeh_pi((__m64*) p, holdValue);
            p += 2;
            holdFramesLeft = holdFrames;
        }
    }

    m_holdFramesLeft = holdFramesLeft;
    _mm_storeh_pi((__m64*) &m_holdValue[0].floatValue, holdValue);
}
#else
void BitCrusherProcessor::process_sse(int* inBuf, int* outBuf, int frames) {}
void BitCrusherProcessor::process_sse(float* inBuf, float* outBuf, int frames) {}
#endif

}

