#include "ck/audio/ringmodprocessor.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/fixedpoint.h"

#if CK_X86_SSE
#  include <emmintrin.h>
#endif

namespace Cki
{


#if CK_X86_SSE
void RingModProcessor::process_sse(int* inBuf, int* outBuf, int frames)
{
    process_default(inBuf, outBuf, frames);
}

void RingModProcessor::process_sse(float* inBuf, float* outBuf, int frames)
{
    if (m_freq == 0.0f)
    {
        return;
    }

    const float b = 4.0f/Math::k_pi;
    const float c = -4.0f/(Math::k_pi * Math::k_pi);
    const float phasePerSample = m_freq/getSampleRate();
    float phase = m_phase;

    const __m128 zero = _mm_setzero_ps();

    const __m128 cv = _mm_set1_ps(c);
    const __m128 bv = _mm_set1_ps(b);

    const __m128 sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31

    float* p = inBuf;
    float* pEnd = p + frames*2;
    while (p < pEnd)
    {
        // faster sine approximation
        //  see http://devmaster.net/posts/9648/fast-and-accurate-sine-cosine
        //
        // y = 4/pi x - 4/pi^2 x^2 = b*x + c*x*x = x*(b + c*x)

        // TODO: instead of vdup, use vector phasev and increment it directly;
        // also could process 4 instead of 2 at a time.

        __m128 phasev = _mm_set1_ps(phase);

        // see http://stackoverflow.com/a/5987631/526829
        __m128 absphasev = _mm_andnot_ps(sign_mask, phasev);

        __m128 tmp = _mm_mul_ps(absphasev, cv);
        tmp = _mm_add_ps(bv, tmp);
        tmp = _mm_mul_ps(tmp, phasev);

        __m128 in = _mm_loadh_pi(zero, (const __m64*) p);
        in = _mm_mul_ps(in, tmp);
        _mm_storeh_pi((__m64*) p, in);

        p += 2;

        phase += phasePerSample;

        if (phase > Math::k_pi)
        {
            phase -= Math::k_2pi;
        }
    }

    m_phase = phase;
}
#else
void RingModProcessor::process_sse(int* inBuf, int* outBuf, int frames) {}
void RingModProcessor::process_sse(float* inBuf, float* outBuf, int frames) {}
#endif


}

