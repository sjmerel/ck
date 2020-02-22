#include "ck/audio/ringmodprocessor.h"
#include "ck/effect.h"
#include "ck/effectparam.h"
#include "ck/core/math.h"
#include "ck/core/fixedpoint.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"

namespace Cki
{


RingModProcessor::RingModProcessor() :
    m_freq(500.0f),
    m_phase(0.0f)
{
}

void RingModProcessor::setParam(int paramId, float value)
{
    switch (paramId)
    {
        case kCkRingModParam_Freq:
            m_freq = Math::max(value, 0.0f);
            break;

        default:
            CK_LOG_ERROR("Ring Mod effect: unknown param ID %d", paramId);
            break;
    }
}

void RingModProcessor::reset()
{
    m_phase = 0.0f;
}

void RingModProcessor::process_default(int* inBuf, int* outBuf, int frames)
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

    int32* p = inBuf;
    int32* pEnd = p + frames*2;
    while (p < pEnd)
    {
        // faster sine approximation
        //  see http://devmaster.net/posts/9648/fast-and-accurate-sine-cosine
        //
        // y = 4/pi x - 4/pi^2 x^2 = b*x + c*x*x = x*(b + c*x)

        // TODO remove abs?  remove branch?

        int64 mod = b + (((int64) c * Math::abs(phase)) >> 24);
        mod = (phase * mod) >> 24;

        *p = (int32) ((*p * mod) >> 24);
        ++p;
        *p = (int32) ((*p * mod) >> 24);
        ++p;

        phase += phasePerSample;

        if (phase > k_pi)
        {
            phase -= k_2pi;
        }
    }

    m_phase = Fixed8_24::toFloat(phase);
}

void RingModProcessor::process_default(float* inBuf, float* outBuf, int frames)
{
    if (m_freq == 0.0f)
    {
        return;
    }

    const float b = 4.0f/Math::k_pi;
    const float c = -4.0f/(Math::k_pi * Math::k_pi);
    const float phasePerSample = m_freq/getSampleRate();

    float phase = m_phase;

    float* p = inBuf;
    float* pEnd = p + frames*2;
    while (p < pEnd)
    {
        // faster sine approximation
        //  see http://devmaster.net/posts/9648/fast-and-accurate-sine-cosine
        //
        // y = 4/pi x - 4/pi^2 x^2 = b*x + c*x*x = x*(b + c*x)

        float mod = phase * (b + (c * Math::abs(phase)));

        *p++ *= mod;
        *p++ *= mod;

        phase += phasePerSample;

        if (phase > Math::k_pi)
        {
            phase -= Math::k_2pi;
        }
    }

    m_phase = phase;
}

}
