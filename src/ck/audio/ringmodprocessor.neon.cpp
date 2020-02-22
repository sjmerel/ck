#include "ck/audio/ringmodprocessor.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/fixedpoint.h"

#if CK_ARM_NEON
#  include <arm_neon.h>
#endif

namespace Cki
{


#if CK_ARM_NEON
void RingModProcessor::process_neon(int* inBuf, int* outBuf, int frames)
{
    if (m_freq == 0.0f)
    {
        return;
    }

    const int32 b = Fixed8_24::fromFloat(4.0f/Math::k_pi);
    const int32 c = Fixed8_24::fromFloat(-4.0f/(Math::k_pi * Math::k_pi));
    const int32 phasePerSample = Fixed8_24::fromFloat(m_freq/getSampleRate());
    int32 phase = Fixed8_24::fromFloat(m_phase);
    const int32 k_pi = Fixed8_24::fromFloat(Math::k_pi);
    const int32 k_2pi = Fixed8_24::fromFloat(Math::k_2pi);

    int32x2_t cv = vdup_n_s32(c);
    int32x2_t bv = vdup_n_s32(b);

    int32* p = inBuf;
    int32* pEnd = p + frames*2;
    while (p < pEnd)
    {
        // faster sine approximation
        //  see http://devmaster.net/posts/9648/fast-and-accurate-sine-cosine
        //
        // y = 4/pi x - 4/pi^2 x^2 = b*x + c*x*x = x*(b + c*x)

        int32x2_t phasev = vdup_n_s32(phase);
        int32x2_t tmp32 = vabs_s32(phasev);
        int64x2_t tmp64 = vmull_s32(tmp32, cv);
        tmp32 = vshrn_n_s64(tmp64, 24);
        tmp32 = vadd_s32(tmp32, bv);
        tmp64 = vmull_s32(tmp32, phasev);
        tmp32 = vshrn_n_s64(tmp64, 24);

        int32x2_t in = vld1_s32(p);
        tmp64 = vmull_s32(in, tmp32);
        in = vshrn_n_s64(tmp64, 24);
        vst1_s32(p, in);

        p += 2;

        phase += phasePerSample;

        if (phase > k_pi)
        {
            phase -= k_2pi;
        }
    }

    m_phase = Fixed8_24::toFloat(phase);
}

void RingModProcessor::process_neon(float* inBuf, float* outBuf, int frames)
{
    if (m_freq == 0.0f)
    {
        return;
    }

    const float b = 4.0f/Math::k_pi;
    const float c = -4.0f/(Math::k_pi * Math::k_pi);
    const float phasePerSample = m_freq/getSampleRate();
    float phase = m_phase;

    float32x2_t cv = vdup_n_f32(c);
    float32x2_t bv = vdup_n_f32(b);

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

        float32x2_t phasev = vdup_n_f32(phase);
        float32x2_t absphasev = vabs_f32(phasev);
        float32x2_t tmp = vmla_f32(bv, absphasev, cv);
        tmp = vmul_f32(tmp, phasev);

        float32x2_t in = vld1_f32(p);
        in = vmul_f32(in, tmp);
        vst1_f32(p, in);

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
void RingModProcessor::process_neon(int* inBuf, int* outBuf, int frames) {}
void RingModProcessor::process_neon(float* inBuf, float* outBuf, int frames) {}
#endif


}

