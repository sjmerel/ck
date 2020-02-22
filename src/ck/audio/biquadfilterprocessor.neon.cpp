#include "ck/audio/biquadfilterprocessor.h"
#include "ck/core/debug.h"

#if CK_ARM_NEON
#  include <arm_neon.h>
#endif

namespace Cki
{


#if CK_ARM_NEON
void BiquadFilterProcessor::process_neon(int* inBuf, int* outBuf, int frames)
{
    if (m_calc)
    {
        calcCoeffs(true);
        m_calc = false;
    }

    // history
    int32x2_t x0 = vld1_s32(&m_x[0][0].fixedValue);
    int32x2_t x1 = vld1_s32(&m_x[1][0].fixedValue);
    int32x2_t y0 = vld1_s32(&m_y[0][0].fixedValue);
    int32x2_t y1 = vld1_s32(&m_y[1][0].fixedValue);

    // coeffs
    int32x2_t b0a0 = vdup_n_s32(m_b0a0.fixedValue);
    int32x2_t b1a0 = vdup_n_s32(m_b1a0.fixedValue);
    int32x2_t b2a0 = vdup_n_s32(m_b2a0.fixedValue);
    int32x2_t a1a0 = vdup_n_s32(m_a1a0.fixedValue);
    int32x2_t a2a0 = vdup_n_s32(m_a2a0.fixedValue);

    int32* p = inBuf;
    int32* pEnd = p + frames*2;
    while (p < pEnd)
    {
        int32x2_t x = vld1_s32(p);

        int64x2_t sum;
        int64x2_t a;
        sum = vmull_s32(b0a0, x);
        a = vmull_s32(b1a0, x0);
        sum = vaddq_s64(sum, a);
        a = vmull_s32(b2a0, x1);
        sum = vaddq_s64(sum, a);
        a = vmull_s32(a1a0, y0);
        sum = vsubq_s64(sum, a);
        a = vmull_s32(a2a0, y1);
        sum = vsubq_s64(sum, a);

        int32x2_t y = vshrn_n_s64(sum, 24);
        vst1_s32(p, y);

        x1 = x0;
        x0 = x;
        y1 = y0;
        y0 = y;

        p += 2;
    }

    vst1_s32(&m_x[0][0].fixedValue, x0);
    vst1_s32(&m_x[1][0].fixedValue, x1);
    vst1_s32(&m_y[0][0].fixedValue, y0);
    vst1_s32(&m_y[1][0].fixedValue, y1);
}

void BiquadFilterProcessor::process_neon(float* inBuf, float* outBuf, int frames)
{
    if (m_calc)
    {
        calcCoeffs(false);
        m_calc = false;
    }

    // history
    float32x2_t x0 = vld1_f32(&m_x[0][0].floatValue);
    float32x2_t x1 = vld1_f32(&m_x[1][0].floatValue);
    float32x2_t y0 = vld1_f32(&m_y[0][0].floatValue);
    float32x2_t y1 = vld1_f32(&m_y[1][0].floatValue);

    // coeffs
    float32x2_t b0a0 = vdup_n_f32(m_b0a0.floatValue);
    float32x2_t b1a0 = vdup_n_f32(m_b1a0.floatValue);
    float32x2_t b2a0 = vdup_n_f32(m_b2a0.floatValue);
    float32x2_t a1a0 = vdup_n_f32(m_a1a0.floatValue);
    float32x2_t a2a0 = vdup_n_f32(m_a2a0.floatValue);

    float* p = inBuf;
    float* pEnd = p + frames*2;
    while (p < pEnd)
    {
        float32x2_t x = vld1_f32(p);

        float32x2_t y;
        float32x2_t a;
        y = vmul_f32(b0a0, x);
        a = vmul_f32(b1a0, x0);
        y = vadd_f32(y, a);
        a = vmul_f32(b2a0, x1);
        y = vadd_f32(y, a);
        a = vmul_f32(a1a0, y0);
        y = vsub_f32(y, a);
        a = vmul_f32(a2a0, y1);
        y = vsub_f32(y, a);

        vst1_f32(p, y);

        x1 = x0;
        x0 = x;
        y1 = y0;
        y0 = y;

        p += 2;
    }

    vst1_f32(&m_x[0][0].floatValue, x0);
    vst1_f32(&m_x[1][0].floatValue, x1);
    vst1_f32(&m_y[0][0].floatValue, y0);
    vst1_f32(&m_y[1][0].floatValue, y1);
}
#else
void BiquadFilterProcessor::process_neon(int* inBuf, int* outBuf, int frames) {}
void BiquadFilterProcessor::process_neon(float* inBuf, float* outBuf, int frames) {}
#endif


}

