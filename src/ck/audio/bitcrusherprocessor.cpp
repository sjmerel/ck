#include "ck/audio/bitcrusherprocessor.h"
#include "ck/effect.h"
#include "ck/effectparam.h"
#include "ck/core/math.h"
#include "ck/core/logger.h"
#include "ck/core/debug.h"

namespace Cki
{


BitCrusherProcessor::BitCrusherProcessor() :
    m_bits(8),
    m_holdMs(1.0f),
    m_holdFramesLeft(0)
{
    m_holdValue[0].fixedValue = m_holdValue[1].fixedValue = 0;
}

void BitCrusherProcessor::setParam(int paramId, float value)
{
    switch (paramId)
    {
        case kCkBitCrusherParam_BitResolution:
            m_bits = Math::clamp((int) value, 1, 24);
            break;

        case kCkBitCrusherParam_HoldMs:
            m_holdMs = Math::max(value, 0.0f);
            break;

        default:
            CK_LOG_ERROR("Bit Crusher effect: unknown param ID %d", paramId);
            break;
    }
}

void BitCrusherProcessor::reset()
{
    m_holdFramesLeft = 0;
}

void BitCrusherProcessor::process(int* inBuf, int* outBuf, int frames)
{
    int holdFrames = (int) (getSampleRate() * m_holdMs * 0.001f);
    if (holdFrames == 0)
    {
        // XXX neon implementation is slow for hold = 0, so using default
        process_default(inBuf, outBuf, frames);
    }
    else
    {
        EffectProcessor::process(inBuf, outBuf, frames);
    }
}

void BitCrusherProcessor::process(float* inBuf, float* outBuf, int frames)
{
    int holdFrames = (int) (getSampleRate() * m_holdMs * 0.001f);
    if (holdFrames == 0)
    {
        // XXX neon implementation is slow for hold = 0, so using default
        process_default(inBuf, outBuf, frames);
    }
    else
    {
        EffectProcessor::process(inBuf, outBuf, frames);
    }
}

void BitCrusherProcessor::process_default(int* inBuf, int* outBuf, int frames)
{
    int holdFrames = (int) (getSampleRate() * m_holdMs * 0.001f);
    int shiftBits = 24 - m_bits;
    uint mask = (0xffffffff >> shiftBits) << shiftBits;

    if (holdFrames == 0 && shiftBits == 0)
    {
        return;
    }

    // copy member vars to locals
    int holdFramesLeft = m_holdFramesLeft;
    int32 holdValue[2] = { m_holdValue[0].fixedValue, m_holdValue[1].fixedValue };

    int32* p = inBuf;
    int32* pEnd = p + frames*2;
    while (p < pEnd)
    {
        if (holdFramesLeft > 0)
        {
            *p++ = holdValue[0];
            *p++ = holdValue[1];
            --holdFramesLeft;
        }
        else
        {
            holdValue[0] = *p & mask;
            *p++ = holdValue[0];
            holdValue[1] = *p & mask;
            *p++ = holdValue[1];
            holdFramesLeft = holdFrames;
        }
    }

    m_holdFramesLeft = holdFramesLeft;
    m_holdValue[0].fixedValue = holdValue[0];
    m_holdValue[1].fixedValue = holdValue[1];
}

void BitCrusherProcessor::process_default(float* inBuf, float* outBuf, int frames)
{
    int holdFrames = (int) (getSampleRate() * m_holdMs * 0.001f);
    int shiftBits = 24 - m_bits;
    uint mask = 0xffffffff << shiftBits;

    if (holdFrames == 0 && shiftBits == 0)
    {
        return;
    }

    // copy member vars to locals
    int holdFramesLeft = m_holdFramesLeft;
    float holdValue[2] = { m_holdValue[0].floatValue, m_holdValue[1].floatValue };

    float* p = inBuf;
    float* pEnd = p + frames*2;
    while (p < pEnd)
    {
        if (holdFramesLeft > 0)
        {
            *p++ = holdValue[0];
            *p++ = holdValue[1];
            --holdFramesLeft;
        }
        else
        {
            int32 intValue;

            intValue = ((int32) (*p * (1<<24))) & mask;
            holdValue[0] = (float) intValue / (1<<24);
            *p++ = holdValue[0];

            intValue = ((int32) (*p * (1<<24))) & mask;
            holdValue[1] = (float) intValue / (1<<24);
            *p++ = holdValue[1];

            holdFramesLeft = holdFrames;
        }
    }

    m_holdFramesLeft = holdFramesLeft;
    m_holdValue[0].floatValue = holdValue[0];
    m_holdValue[1].floatValue = holdValue[1];
}


}
