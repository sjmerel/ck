#include "ck/audio/distortionprocessor.h"
#include "ck/core/debug.h"
#include "ck/core/fixedpoint.h"

#if CK_ARM_NEON
#  include <arm_neon.h>
#endif

namespace Cki
{


#if CK_ARM_NEON
void DistortionProcessor::process_neon(int* inBuf, int* outBuf, int frames)
{
    int32 drive = Fixed8_24::fromFloat(m_drive);
    int32 offset = Fixed8_24::fromFloat(m_offset);
    int32 oneThird = Fixed8_24::fromFloat(0.33333f);

    int32x2_t drive_v = vdup_n_s32(drive);
    int32x4_t offset_v = vdupq_n_s32(offset);
    int32x2_t oneThird_v = vdup_n_s32(oneThird);

    int samples = frames*2;
    int32* p = inBuf;
    int32* pEnd = p + samples;
    int32* pEnd4 = p + (samples & ~3);
    while (p < pEnd4)
    {
        int32x4_t in = vld1q_s32(p);

        int32x4_t tmp32;
        int32x2_t tmp32l;
        int32x2_t tmp32h;
        int64x2_t tmp64l;
        int64x2_t tmp64h;

        tmp32 = vaddq_s32(in, offset_v);
        tmp32l = vget_low_s32(tmp32);
        tmp32h = vget_high_s32(tmp32);
        tmp64l = vmull_s32(tmp32l, drive_v);
        tmp64h = vmull_s32(tmp32h, drive_v);
        int32x2_t xl = vshrn_n_s64(tmp64l, 24);
        int32x2_t xh = vshrn_n_s64(tmp64h, 24);

        // xx
        tmp64l = vmull_s32(xl, xl);
        tmp64h = vmull_s32(xh, xh);
        tmp32l = vshrn_n_s64(tmp64l, 24);
        tmp32h = vshrn_n_s64(tmp64h, 24);

        // xxx
        tmp64l = vmull_s32(tmp32l, xl);
        tmp64h = vmull_s32(tmp32h, xh);
        tmp32l = vshrn_n_s64(tmp64l, 24);
        tmp32h = vshrn_n_s64(tmp64h, 24);

        // xxx3
        tmp64l = vmull_s32(tmp32l, oneThird_v);
        tmp64h = vmull_s32(tmp32h, oneThird_v);
        tmp32l = vshrn_n_s64(tmp64l, 24);
        tmp32h = vshrn_n_s64(tmp64h, 24);

        tmp32 = vcombine_s32(tmp32l, tmp32h);
        int32x4_t x = vcombine_s32(xl, xh);

        tmp32 = vsubq_s32(x, tmp32);
        vst1q_s32(p, tmp32);
        p += 4;
    }

    while (p < pEnd)
    {
        int32 x = (int32) (((int64) (*p + offset) * drive) >> 24);
        int32 xx = (int32) (((int64) x * x) >> 24);
        int32 xxx = (int32) (((int64) xx * x) >> 24);
        int32 xxx3 = (int32) (((int64) xxx * oneThird) >> 24);
        *p++ = x - xxx3;
    }
}

void DistortionProcessor::process_neon(float* inBuf, float* outBuf, int frames)
{
    float drive = m_drive;
    float offset = m_offset;
    float oneThird = 0.33333f;

    float32x4_t drive_v = vdupq_n_f32(drive);
    float32x4_t offset_v = vdupq_n_f32(offset);
    float32x4_t oneThird_v = vdupq_n_f32(oneThird);

    int samples = frames*2;
    float32* p = inBuf;
    float32* pEnd = p + samples;
    float32* pEnd4 = p + (samples & ~3);
    while (p < pEnd4)
    {
        float32x4_t in = vld1q_f32(p);

        float32x4_t tmp;

        tmp = vaddq_f32(in, offset_v);
        float32x4_t x = vmulq_f32(tmp, drive_v);

        // xx
        tmp = vmulq_f32(x, x);

        // xxx
        tmp = vmulq_f32(tmp, x);

        // xxx3
        tmp = vmulq_f32(tmp, oneThird_v);

        tmp = vsubq_f32(x, tmp);
        vst1q_f32(p, tmp);
        p += 4;
    }

    while (p < pEnd)
    {
        float x = (*p + offset) * drive;
        *p++ = x - (x * x * x * oneThird);
    }
}
#else
void DistortionProcessor::process_neon(int* inBuf, int* outBuf, int frames) {}
void DistortionProcessor::process_neon(float* inBuf, float* outBuf, int frames) {}
#endif


}

