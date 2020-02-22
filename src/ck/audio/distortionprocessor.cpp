#include "ck/audio/distortionprocessor.h"
#include "ck/effect.h"
#include "ck/effectparam.h"
#include "ck/core/math.h"
#include "ck/core/fixedpoint.h"
#include "ck/core/logger.h"

namespace Cki
{


DistortionProcessor::DistortionProcessor() :
    m_drive(1.0f),
    m_offset(0.0f)
{
}

void DistortionProcessor::setParam(int paramId, float value)
{
    switch (paramId)
    {
        case kCkDistortionParam_Drive:
            m_drive = value;
            break;

        case kCkDistortionParam_Offset:
            m_offset = value;
            break;

        default:
            CK_LOG_ERROR("Distortion effect: unknown param ID %d", paramId);
            break;
    }
}

void DistortionProcessor::reset()
{
}

void DistortionProcessor::process_default(int* inBuf, int* outBuf, int frames)
{
    // y(x) = tanh(x) =~ (0.1076*x^3 + 3.029*x)/(x^2 + 3.124)
    // y(x) = atan(x) =~ .75*(((x*1.3)^2+1)^.5*1.65-1.65)/x
    // y(x) = x - x^3 / 3

    int32 drive = Fixed8_24::fromFloat(m_drive);
    int32 offset = Fixed8_24::fromFloat(m_offset);
    int32 oneThird = Fixed8_24::fromFloat(0.33333f);

    int32* p = inBuf;
    int32* pEnd = p + frames*2;
    while (p < pEnd)
    {
        int32 x = (int32) (((int64) (*p + offset) * drive) >> 24);
        int32 xx = (int32) (((int64) x * x) >> 24);
        int32 xxx = (int32) (((int64) xx * x) >> 24);
        int32 xxx3 = (int32) (((int64) xxx * oneThird) >> 24);
        *p++ = x - xxx3;
    }
}

void DistortionProcessor::process_default(float* inBuf, float* outBuf, int frames)
{
    // y(x) = tanh(x) =~ (0.1076*x^3 + 3.029*x)/(x^2 + 3.124)
    // y(x) = atan(x) =~ .75*(((x*1.3)^2+1)^.5*1.65-1.65)/x
    // y(x) = x - x^3 / 3

    float drive = m_drive;
    float offset = m_offset;
    float oneThird = 0.33333f;

    float* p = inBuf;
    float* pEnd = p + frames*2;
    while (p < pEnd)
    {
        float x = (*p + offset) * drive;
        *p++ = x - (x * x * x * oneThird);
    }
}


}

