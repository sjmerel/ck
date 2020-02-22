#include "ck/audio/distortionprocessor.h"
#include "ck/core/debug.h"
#include "ck/core/fixedpoint.h"

#if CK_X86_SSE
#  include <emmintrin.h>
#endif

namespace Cki
{


#if CK_X86_SSE
void DistortionProcessor::process_sse(int* inBuf, int* outBuf, int frames)
{
    process_default(inBuf, outBuf, frames);
}

void DistortionProcessor::process_sse(float* inBuf, float* outBuf, int frames)
{
    float drive = m_drive;
    float offset = m_offset;
    float oneThird = 0.33333f;

    __m128 drive_v = _mm_set1_ps(drive);
    __m128 offset_v = _mm_set1_ps(offset);
    __m128 oneThird_v = _mm_set1_ps(oneThird);

    int samples = frames*2;
    float32* p = inBuf;
    float32* pEnd = p + samples;
    float32* pEnd4 = p + (samples & ~3);
    while (p < pEnd4)
    {
        __m128 in = _mm_load_ps(p);

        __m128 tmp;

        tmp = _mm_add_ps(in, offset_v);
        __m128 x = _mm_mul_ps(tmp, drive_v);

        // xx
        tmp = _mm_mul_ps(x, x);

        // xxx
        tmp = _mm_mul_ps(tmp, x);

        // xxx3
        tmp = _mm_mul_ps(tmp, oneThird_v);

        tmp = _mm_sub_ps(x, tmp);
        _mm_store_ps(p, tmp);
        p += 4;
    }

    while (p < pEnd)
    {
        float x = (*p + offset) * drive;
        *p++ = x - (x * x * x * oneThird);
    }
}
#else
void DistortionProcessor::process_sse(int* inBuf, int* outBuf, int frames) {}
void DistortionProcessor::process_sse(float* inBuf, float* outBuf, int frames) {}
#endif


}

