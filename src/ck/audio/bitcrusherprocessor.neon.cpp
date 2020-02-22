#include "ck/audio/bitcrusherprocessor.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"

#if CK_ARM_NEON
#  include <arm_neon.h>
#endif

namespace Cki
{


#if CK_ARM_NEON
void BitCrusherProcessor::process_neon(int* inBuf, int* outBuf, int frames)
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

    int32* p = inBuf;
    int32* pEnd = p + frames*2;

    int32x2_t maskv = vdup_n_s32(mask);
    int32x2_t holdValue = vld1_s32(&m_holdValue[0].fixedValue);

    while (p < pEnd)
    {
        if (holdFramesLeft > 0)
        {
            vst1_s32(p, holdValue);
            p += 2;
            --holdFramesLeft;
        }
        else
        {
            holdValue = vld1_s32(p);
            holdValue = vand_s32(holdValue, maskv);
            vst1_s32(p, holdValue);
            p += 2;
            holdFramesLeft = holdFrames;
        }
    }

    m_holdFramesLeft = holdFramesLeft;
    vst1_s32(&m_holdValue[0].fixedValue, holdValue);
}

void BitCrusherProcessor::process_neon(float* inBuf, float* outBuf, int frames)
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

    int32x2_t maskv = vdup_n_s32(mask);
    float32x2_t holdValue = vld1_f32(&m_holdValue[0].floatValue);

    while (p < pEnd)
    {
        if (holdFramesLeft > 0)
        {
            vst1_f32(p, holdValue);
            p += 2;
            --holdFramesLeft;
        }
        else
        {
            holdValue = vld1_f32(p);
            int32x2_t intValue = vcvt_n_s32_f32(holdValue, 24);
            intValue = vand_s32(intValue, maskv);
            holdValue = vcvt_n_f32_s32(intValue, 24);
            vst1_f32(p, holdValue);
            p += 2;
            holdFramesLeft = holdFrames;
        }
    }

    m_holdFramesLeft = holdFramesLeft;
    vst1_f32(&m_holdValue[0].floatValue, holdValue);
}
#else
void BitCrusherProcessor::process_neon(int* inBuf, int* outBuf, int frames) {}
void BitCrusherProcessor::process_neon(float* inBuf, float* outBuf, int frames) {}
#endif

}

