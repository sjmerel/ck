#include "ck/audio/biquadfilterprocessor.h"
#include "ck/core/debug.h"

#if CK_X86_SSE
#  include <emmintrin.h>
#endif

namespace Cki
{


#if CK_X86_SSE
void BiquadFilterProcessor::process_sse(int* inBuf, int* outBuf, int frames)
{
    return process_default(inBuf, outBuf, frames);
}

void BiquadFilterProcessor::process_sse(float* inBuf, float* outBuf, int frames)
{
    if (m_calc)
    {
        calcCoeffs(false);
        m_calc = false;
    }

    __m128 zero = _mm_setzero_ps();

    // history
    __m128 x0 = _mm_loadh_pi(zero, (const __m64*) &m_x[0][0].floatValue);
    __m128 x1 = _mm_loadh_pi(zero, (const __m64*) &m_x[1][0].floatValue);
    __m128 y0 = _mm_loadh_pi(zero, (const __m64*) &m_y[0][0].floatValue);
    __m128 y1 = _mm_loadh_pi(zero, (const __m64*) &m_y[1][0].floatValue);

    // coeffs
    __m128 b0a0 = _mm_set1_ps(m_b0a0.floatValue);
    __m128 b1a0 = _mm_set1_ps(m_b1a0.floatValue);
    __m128 b2a0 = _mm_set1_ps(m_b2a0.floatValue);
    __m128 a1a0 = _mm_set1_ps(m_a1a0.floatValue);
    __m128 a2a0 = _mm_set1_ps(m_a2a0.floatValue);

    float* p = inBuf;
    float* pEnd = p + frames*2;
    while (p < pEnd)
    {
        __m128 x = _mm_loadh_pi(zero, (const __m64*) p);

        __m128 y;
        __m128 a;
        y = _mm_mul_ps(b0a0, x);
        a = _mm_mul_ps(b1a0, x0);
        y = _mm_add_ps(y, a);
        a = _mm_mul_ps(b2a0, x1);
        y = _mm_add_ps(y, a);
        a = _mm_mul_ps(a1a0, y0);
        y = _mm_sub_ps(y, a);
        a = _mm_mul_ps(a2a0, y1);
        y = _mm_sub_ps(y, a);

        _mm_storeh_pi((__m64*) p, y);

        x1 = x0;
        x0 = x;
        y1 = y0;
        y0 = y;

        p += 2;
    }

    _mm_storeh_pi((__m64*) &m_x[0][0].floatValue, x0);
    _mm_storeh_pi((__m64*) &m_x[1][0].floatValue, x1);
    _mm_storeh_pi((__m64*) &m_y[0][0].floatValue, y0);
    _mm_storeh_pi((__m64*) &m_y[1][0].floatValue, y1);
}
#else
void BiquadFilterProcessor::process_sse(int* inBuf, int* outBuf, int frames) {}
void BiquadFilterProcessor::process_sse(float* inBuf, float* outBuf, int frames) {}
#endif


}


