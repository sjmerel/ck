#include "ck/audio/audioutil.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/fixedpoint.h"
#include "ck/audio/volumematrix.h"

#if CK_ARM_NEON

#include <arm_neon.h>

// On Android, -mfpu=neon generates code that cannot run on non-NEON devices,
// so it must be compiled separately in this file.

namespace Cki
{


namespace AudioUtil
{
    ////////////////////////////////////////
    // convert float to int32

    void convertF32toI32_neon(const float* inBuf, int32* outBuf, int samples)
    {
        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (samples & ~3);
        const float* pInEnd = inBuf + samples;
        int32* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            float32x4_t in = vld1q_f32(pIn);
            int32x4_t out = vcvtq_n_s32_f32(in, 24);
            vst1q_s32(pOut, out);
            pIn += 4;
            pOut += 4;
        }

        while (pIn < pInEnd)
        {
            *pOut++ = (int32) (*pIn++ * (1 << 24));
        }
    }


    ////////////////////////////////////////
    // convert float to int16

    void convertF32toI16_neon(const float* inBuf, int16* outBuf, int samples)
    {
        const float* pIn = inBuf;
        const float* pInEnd8 = inBuf + (samples & ~7);
        const float* pInEnd = inBuf + samples;
        int16* pOut = outBuf;

        float32x4_t mul = vdupq_n_f32((float) CK_INT16_MAX);

        while (pIn < pInEnd8)
        {
            float32x4_t in;
            int32x4_t tmp;

            in = vld1q_f32(pIn);
            in = vmulq_f32(in, mul);
            tmp = vcvtq_s32_f32(in);
            int16x4_t out0 = vmovn_s32(tmp);
            pIn += 4;

            in = vld1q_f32(pIn);
            in = vmulq_f32(in, mul);
            tmp = vcvtq_s32_f32(in);
            int16x4_t out1 = vmovn_s32(tmp);
            pIn += 4;

            int16x8_t out = vcombine_s16(out0, out1);
            vst1q_s16(pOut, out);
            pOut += 8;
        }

        while (pIn < pInEnd)
        {
            *pOut++ = (int16) (*pIn++ * CK_INT16_MAX);
        }
    }


    ////////////////////////////////////////
    // convert int32 to float

    void convertI32toF32_neon(const int32* inBuf, float* outBuf, int samples)
    {
        const int32* pIn = inBuf;
        const int32* pInEnd4 = inBuf + (samples & ~3);
        const int32* pInEnd = inBuf + samples;
        float* pOut = outBuf;
        float factor = 1.0f / (1 << 24);

        while (pIn < pInEnd4)
        {
            int32x4_t in = vld1q_s32(pIn);
            float32x4_t out = vcvtq_n_f32_s32(in, 24);
            vst1q_f32(pOut, out);
            pIn += 4;
            pOut += 4;
        }

        while (pIn < pInEnd)
        {
            *pOut++ = (float) *pIn++ * factor;
        }
    }


    ////////////////////////////////////////
    // convert int32 to int16

    void convertI32toI16_neon(const int32* inBuf, int16* outBuf, int samples)
    {
        const int32* pIn = inBuf;
        const int32* pInEnd8 = inBuf + (samples & ~7);
        const int32* pInEnd = inBuf + samples;
        int16* pOut = outBuf;

        while (pIn < pInEnd8)
        {
            int32x4_t in0 = vld1q_s32(pIn);
            int16x4_t out0 = vshrn_n_s32(in0, 9);
            pIn += 4;
            int32x4_t in1 = vld1q_s32(pIn);
            int16x4_t out1 = vshrn_n_s32(in1, 9);
            pIn += 4;
            int16x8_t out = vcombine_s16(out0, out1);
            vst1q_s16(pOut, out);
            pOut += 8;
        }

        while (pIn < pInEnd)
        {
            *pOut++ = (int16) (*pIn++ >> 9);
        }
    }


    ////////////////////////////////////////
    // convert int16 to float

    void convertI16toF32_neon(const int16* inBuf, float* outBuf, int samples)
    {
        // iterate backwards so we can do it in-place

        const int16* pIn = inBuf + samples - 1;
        const int16* pInEnd8 = inBuf + (samples & ~7) - 1;
        float* pOut = outBuf + samples - 1;
        float factor = 1.0f / CK_INT16_MAX;

        while (pIn > pInEnd8)
        {
            *pOut-- = ((float) *pIn--) * factor;
        }
        pIn -= 7;
        pOut -= 3;

        float32x4_t factorv = vdupq_n_f32(factor);
        while (pIn >= inBuf)
        {
            // load int16s
            int16x8_t in16 = vld1q_s16(pIn);

            // split
            int16x4_t in16l = vget_low_s16(in16);
            int16x4_t in16h = vget_high_s16(in16);

            // widen to int32s
            int32x4_t in32l = vmovl_s16(in16l);
            int32x4_t in32h = vmovl_s16(in16h);

            // convert to float
            float32x4_t outl = vcvtq_f32_s32(in32l);
            float32x4_t outh = vcvtq_f32_s32(in32h);

            // scale
            outl = vmulq_f32(outl, factorv);
            outh = vmulq_f32(outh, factorv);

            // store 
            vst1q_f32(pOut, outh);
            pOut -= 4;
            vst1q_f32(pOut, outl);
            pOut -= 4;

            pIn -= 8;
        }

        CK_ASSERT(pIn == inBuf - 8);
        CK_ASSERT(pOut == outBuf - 4);
    }


    ////////////////////////////////////////
    // convert int16 to int32

    void convertI16toI32_neon(const int16* inBuf, int32* outBuf, int samples)
    {
        const int16* pIn = inBuf + samples - 1;
        const int16* pInEnd8 = inBuf + (samples & ~7) - 1;
        int32* pOut = outBuf + samples - 1;

        while (pIn > pInEnd8)
        {
            *pOut-- = ((int32) *pIn--) << 9;
        }
        pIn -= 7;
        pOut -= 3;

        while (pIn >= inBuf)
        {
            int16x8_t in = vld1q_s16(pIn);
            int16x4_t inl = vget_low_s16(in);
            int16x4_t inh = vget_high_s16(in);
            int32x4_t outl = vshll_n_s16(inl, 9);
            int32x4_t outh = vshll_n_s16(inh, 9);
            vst1q_s32(pOut, outh);
            pOut -= 4;
            vst1q_s32(pOut, outl);
            pOut -= 4;
            pIn -= 8;
        }

        CK_ASSERT(pIn == inBuf - 8);
        CK_ASSERT(pOut == outBuf - 4);
    }


    ////////////////////////////////////////
    // convert int8 to float

    void convertI8toF32_neon(const int8* inBuf, float* outBuf, int samples)
    {
        // iterate backwards so we can do it in-place
        const int8* pIn = inBuf + samples - 1;
        const int8* pInEnd16 = inBuf + (samples & ~15) - 1;
        float* pOut = outBuf + samples - 1;
        float factor = 1.0f / CK_INT8_MAX;

        while (pIn > pInEnd16)
        {
            *pOut-- = ((float) *pIn--) * factor;
        }
        pIn -= 15;
        pOut -= 3;

        float32x4_t factorv = vdupq_n_f32(factor);
        while (pIn >= inBuf)
        {
            // load int8s
            int8x16_t in8 = vld1q_s8(pIn);

            // split
            int8x8_t in8l = vget_low_s8(in8);
            int8x8_t in8h = vget_high_s8(in8);

            // widen to int16s
            int16x8_t in16l = vmovl_s8(in8l);
            int16x8_t in16h = vmovl_s8(in8h);

            // split 
            int16x4_t in16ll = vget_low_s16(in16l);
            int16x4_t in16lh = vget_high_s16(in16l);
            int16x4_t in16hl = vget_low_s16(in16h);
            int16x4_t in16hh = vget_high_s16(in16h);

            // widen to int32s
            int32x4_t in32ll = vmovl_s16(in16ll);
            int32x4_t in32lh = vmovl_s16(in16lh);
            int32x4_t in32hl = vmovl_s16(in16hl);
            int32x4_t in32hh = vmovl_s16(in16hh);

            // convert to float
            float32x4_t outll = vcvtq_f32_s32(in32ll);
            float32x4_t outlh = vcvtq_f32_s32(in32lh);
            float32x4_t outhl = vcvtq_f32_s32(in32hl);
            float32x4_t outhh = vcvtq_f32_s32(in32hh);

            // scale
            outll = vmulq_f32(outll, factorv);
            outlh = vmulq_f32(outlh, factorv);
            outhl = vmulq_f32(outhl, factorv);
            outhh = vmulq_f32(outhh, factorv);

            // store 
            vst1q_f32(pOut, outhh);
            pOut -= 4;
            vst1q_f32(pOut, outhl);
            pOut -= 4;
            vst1q_f32(pOut, outlh);
            pOut -= 4;
            vst1q_f32(pOut, outll);
            pOut -= 4;

            pIn -= 16;
        }

        CK_ASSERT(pIn == inBuf - 16);
        CK_ASSERT(pOut == outBuf - 4);
    }


    ////////////////////////////////////////
    // convert int8 to int32

    void convertI8toI32_neon(const int8* inBuf, int32* outBuf, int samples)
    {
        const int8* pIn = inBuf + samples - 1;
        const int8* pInEnd16 = inBuf + (samples & ~15) - 1;
        int32* pOut = outBuf + samples - 1;

        while (pIn > pInEnd16)
        {
            *pOut-- = ((int32) *pIn--) << 17;
        }
        pIn -= 15;
        pOut -= 3;

        while (pIn >= inBuf)
        {
            // load int8s
            int8x16_t in8 = vld1q_s8(pIn);

            // split
            int8x8_t in8l = vget_low_s8(in8);
            int8x8_t in8h = vget_high_s8(in8);

            // widen to int16s
            int16x8_t in16l = vmovl_s8(in8l);
            int16x8_t in16h = vmovl_s8(in8h);

            // split 
            int16x4_t in16ll = vget_low_s16(in16l);
            int16x4_t in16lh = vget_high_s16(in16l);
            int16x4_t in16hl = vget_low_s16(in16h);
            int16x4_t in16hh = vget_high_s16(in16h);

            // widen to int32s
            int32x4_t in32ll = vmovl_s16(in16ll);
            int32x4_t in32lh = vmovl_s16(in16lh);
            int32x4_t in32hl = vmovl_s16(in16hl);
            int32x4_t in32hh = vmovl_s16(in16hh);

            // shift 
            int32x4_t outll = vshlq_n_s32(in32ll, 17);
            int32x4_t outlh = vshlq_n_s32(in32lh, 17);
            int32x4_t outhl = vshlq_n_s32(in32hl, 17);
            int32x4_t outhh = vshlq_n_s32(in32hh, 17);

            // store 
            vst1q_s32(pOut, outhh);
            pOut -= 4;
            vst1q_s32(pOut, outhl);
            pOut -= 4;
            vst1q_s32(pOut, outlh);
            pOut -= 4;
            vst1q_s32(pOut, outll);
            pOut -= 4;

            pIn -= 16;
        }

        CK_ASSERT(pIn == inBuf - 16);
        CK_ASSERT(pOut == outBuf - 4);
    }


    ////////////////////////////////////////
    // mix

    void mix_neon(const int32* __restrict inBuf, int32* __restrict outBuf, int samples)
    {
        CK_ASSERT(inBuf != outBuf);
        const int32* pIn = inBuf;
        const int32* pInEnd4 = inBuf + (samples & ~3);
        const int32* pInEnd = inBuf + samples;
        int32* pOut = outBuf;

        int32x4_t in;
        int32x4_t out;

        while (pIn < pInEnd4)
        {
            in = vld1q_s32(pIn);
            out = vld1q_s32(pOut);
            out = vaddq_s32(in, out);
            vst1q_s32(pOut, out);
            pOut += 4;
            pIn += 4;
        }

        while (pIn < pInEnd)
        {
            *pOut++ += *pIn++;
        }
    }

    void mix_neon(const float* __restrict inBuf, float* __restrict outBuf, int samples)
    {
        CK_ASSERT(inBuf != outBuf);
        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (samples & ~3);
        const float* pInEnd = inBuf + samples;
        float* pOut = outBuf;

        float32x4_t in;
        float32x4_t out;

        while (pIn < pInEnd4)
        {
            in = vld1q_f32(pIn);
            out = vld1q_f32(pOut);
            out = vaddq_f32(in, out);
            vst1q_f32(pOut, out);
            pOut += 4;
            pIn += 4;
        }

        while (pIn < pInEnd)
        {
            *pOut++ += *pIn++;
        }
    }

    void mixVol_neon(const int32* __restrict inBuf, float inVol, int32* __restrict outBuf, float outVol, int samples)
    {
        CK_ASSERT(inBuf != outBuf);

        int32 inVoli = Fixed8_24::fromFloat(inVol);
        int32 outVoli = Fixed8_24::fromFloat(outVol);

        int32x2_t outVolv = vdup_n_s32(outVoli);
        int32x2_t inVolv = vdup_n_s32(inVoli);

        const int32* pIn = inBuf;
        const int32* pInEnd4 = inBuf + (samples & ~3);
        const int32* pInEnd = inBuf + samples;
        int32* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            int32x4_t in = vld1q_s32(pIn);
            int32x4_t out = vld1q_s32(pOut);

            int32x2_t in_low = vget_low_s32(in);
            int32x2_t in_high = vget_high_s32(in);
            int32x2_t out_low = vget_low_s32(out);
            int32x2_t out_high = vget_high_s32(out);

            int64x2_t tmp_low = vmull_s32(in_low, inVolv);
            int64x2_t tmp_high = vmull_s32(in_high, inVolv);
            tmp_low = vmlal_s32(tmp_low, out_low, outVolv);
            tmp_high = vmlal_s32(tmp_high, out_high, outVolv);
            out_low = vshrn_n_s64(tmp_low, 24);
            out_high = vshrn_n_s64(tmp_high, 24);
            out = vcombine_s32(out_low, out_high);

            vst1q_s32(pOut, out);
            pOut += 4;
            pIn += 4;
        }

        while (pIn < pInEnd)
        {
            int64 in = *pIn;
            int64 out = *pOut;
            *pOut = (int32) ((in * inVoli + out * outVoli) >> 24);
            ++pIn;
            ++pOut;
        }
    }

    void mixVol_neon(const float* __restrict inBuf, float inVol, float* __restrict outBuf, float outVol, int samples)
    {
        CK_ASSERT(inBuf != outBuf);

        float32x4_t outVolv = vdupq_n_f32(outVol);
        float32x4_t inVolv = vdupq_n_f32(inVol);

        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (samples & ~3);
        const float* pInEnd = inBuf + samples;
        float* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            float32x4_t in = vld1q_f32(pIn);
            float32x4_t out = vld1q_f32(pOut);
            in = vmulq_f32(in, inVolv);
            out = vmlaq_f32(in, out, outVolv);
            vst1q_f32(pOut, out);
            pOut += 4;
            pIn += 4;
        }

        while (pIn < pInEnd)
        {
            *pOut = *pIn * inVol + *pOut * outVol;
            ++pIn;
            ++pOut;
        }
    }


    ////////////////////////////////////////
    // convert to stereo

    void convertToStereo_neon(const int32* inBuf, int32* outBuf, int frames)
    {
        CK_ASSERT(inBuf != outBuf);
        const int32* pIn = inBuf;
        const int32* pInEnd4 = inBuf + (frames & ~3);
        const int32* pInEnd = inBuf + frames;
        int32* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            int32x4_t in = vld1q_s32(pIn);
            int32x4x2_t out;
            out.val[0] = in;
            out.val[1] = in;

            vst2q_s32(pOut, out);

            pIn += 4;
            pOut += 8;
        }

        while (pIn < pInEnd)
        {
            int32 in = *pIn++;
            *pOut++ = in;
            *pOut++ = in;
        }
    }

    void convertToStereo_neon(const float* inBuf, float* outBuf, int frames)
    {
        CK_ASSERT(inBuf != outBuf);
        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (frames & ~3);
        const float* pInEnd = inBuf + frames;
        float* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            float32x4_t in = vld1q_f32(pIn);
            float32x4x2_t out;
            out.val[0] = in;
            out.val[1] = in;

            vst2q_f32(pOut, out);

            pIn += 4;
            pOut += 8;
        }

        while (pIn < pInEnd)
        {
            float32 in = *pIn++;
            *pOut++ = in;
            *pOut++ = in;
        }
    }


    ////////////////////////////////////////
    // convert to stereo with scale

    void convertToStereoScale_neon(const int32* inBuf, int32* outBuf, int frames, float volume)
    {
        int64 v = Fixed8_24::fromFloat(volume);
        int32x2_t vv = vdup_n_s32((int32) v);

        const int32* pIn = inBuf;
        const int32* pInEnd4 = inBuf + (frames & ~3);
        const int32* pInEnd = inBuf + frames;
        int32* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            int32x4_t in = vld1q_s32(pIn);

            int32x2_t in_high = vget_high_s32(in);
            int32x2_t in_low = vget_low_s32(in);
            int64x2_t tmp_low = vmull_s32(in_low, vv);
            int64x2_t tmp_high = vmull_s32(in_high, vv);
            in_low = vshrn_n_s64(tmp_low, 24);
            in_high = vshrn_n_s64(tmp_high, 24);
            in = vcombine_s32(in_low, in_high);

            int32x4x2_t out;
            out.val[0] = in;
            out.val[1] = in;
            vst2q_s32(pOut, out);

            pIn += 4;
            pOut += 8;
        }

        while (pIn < pInEnd)
        {
            int32 out = (int32) ((*pIn++ * v) >> 24);
            *pOut++ = out;
            *pOut++ = out;
        }
    }

    void convertToStereoScale_neon(const float* inBuf, float* outBuf, int frames, float volume)
    {
        float32x4_t v = vdupq_n_f32(volume);

        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (frames & ~3);
        const float* pInEnd = inBuf + frames;
        float* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            float32x4_t in = vld1q_f32(pIn);
            in = vmulq_f32(in, v);

            float32x4x2_t out;
            out.val[0] = in;
            out.val[1] = in;
            vst2q_f32(pOut, out);

            pIn += 4;
            pOut += 8;
        }

        while (pIn < pInEnd)
        {
            float out = *pIn++ * volume;
            *pOut++ = out;
            *pOut++ = out;
        }
    }

    ////////////////////////////////////////
    // scale

    void scale_neon(const int32* inBuf, int32* outBuf, int samples, float volume)
    {
        int64 voli = Fixed8_24::fromFloat(volume);
        int32x2_t volv = vdup_n_s32((int32) voli);

        const int32* pIn = inBuf;
        const int32* pInEnd4 = inBuf + (samples & ~3);
        const int32* pInEnd = inBuf + samples;
        int32* pOut = outBuf;
        while (pIn < pInEnd4)
        {
            int32x4_t in = vld1q_s32(pIn);
            int32x2_t in_low = vget_low_s32(in);
            int32x2_t in_high = vget_high_s32(in);
            int64x2_t tmp_low = vmull_s32(in_low, volv);
            int64x2_t tmp_high = vmull_s32(in_high, volv);
            in_low = vshrn_n_s64(tmp_low, 24);
            in_high = vshrn_n_s64(tmp_high, 24);
            in = vcombine_s32(in_low, in_high);
            vst1q_s32(pOut, in);
            pIn += 4;
            pOut += 4;
        }

        while (pIn < pInEnd)
        {
            *pOut++ = (int32) ((*pIn++ * voli) >> 24);
        }
    }

    void scale_neon(const float* inBuf, float* outBuf, int samples, float volume)
    {
        float32x4_t v = vdupq_n_f32(volume);

        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (samples & ~3);
        const float* pInEnd = inBuf + samples;
        float* pOut = outBuf;
        while (pIn < pInEnd4)
        {
            float32x4_t in = vld1q_f32(pIn);
            in = vmulq_f32(in, v);
            vst1q_f32(pOut, in);
            pIn += 4;
            pOut += 4;
        }

        while (pIn < pInEnd)
        {
            *pOut++ = *pIn++ * volume;
        }
    }


    ////////////////////////////////////////
    // stereo pan

    void stereoPan_neon(int32* buf, int frames, const VolumeMatrix& volume)
    {
        int32 ll = Fixed8_24::fromFloat(volume.ll);
        int32 lr = Fixed8_24::fromFloat(volume.lr);
        int32 rl = Fixed8_24::fromFloat(volume.rl);
        int32 rr = Fixed8_24::fromFloat(volume.rr);

        int32* p = buf;
        const int32* pEnd8 = buf + ((frames * 2) & ~7);
        const int32* pEnd = buf + frames * 2;

        // load constants
        int32x2_t llv = vdup_n_s32(ll);
        int32x2_t lrv = vdup_n_s32(lr);
        int32x2_t rlv = vdup_n_s32(rl);
        int32x2_t rrv = vdup_n_s32(rr);

        while (p < pEnd8)
        {
            // load 4 samples for each channel, deinterleaving
            int32x4x2_t in = vld2q_s32(p);

            int32x2_t in_left_low = vget_low_s32(in.val[0]);
            int32x2_t in_right_low = vget_low_s32(in.val[1]);
            int32x2_t in_left_high = vget_high_s32(in.val[0]);
            int32x2_t in_right_high = vget_high_s32(in.val[1]);

            int32x4x2_t out;
            int32x2_t out_low;
            int32x2_t out_high;
            int64x2_t tmp;

            // left
            tmp = vmull_s32(in_left_low, llv);
            tmp = vmlal_s32(tmp, in_right_low, lrv);
            out_low = vshrn_n_s64(tmp, 24);

            tmp = vmull_s32(in_left_high, llv);
            tmp = vmlal_s32(tmp, in_right_high, lrv);
            out_high = vshrn_n_s64(tmp, 24);

            out.val[0] = vcombine_s32(out_low, out_high);

            // right
            tmp = vmull_s32(in_left_low, rlv);
            tmp = vmlal_s32(tmp, in_right_low, rrv);
            out_low = vshrn_n_s64(tmp, 24);

            tmp = vmull_s32(in_left_high, rlv);
            tmp = vmlal_s32(tmp, in_right_high, rrv);
            out_high = vshrn_n_s64(tmp, 24);

            out.val[1] = vcombine_s32(out_low, out_high);

            vst2q_s32(p, out);

            p += 8;
        }

        while (p < pEnd)
        {
            int64 in_l = *p;
            int64 in_r = *(p+1);

            *p++ = (int32) ((in_l * ll + in_r * lr) >> 24);
            *p++ = (int32) ((in_l * rl + in_r * rr) >> 24);
        }
    }

    void stereoPan_neon(float* buf, int frames, const VolumeMatrix& volume)
    {
        float ll = volume.ll;
        float lr = volume.lr;
        float rl = volume.rl;
        float rr = volume.rr;

        float* p = buf;
        const float* pEnd8 = buf + ((frames * 2) & ~7);
        const float* pEnd = buf + frames * 2;

        // load constants
        float32x4_t llv = vdupq_n_f32(ll);
        float32x4_t lrv = vdupq_n_f32(lr);
        float32x4_t rlv = vdupq_n_f32(rl);
        float32x4_t rrv = vdupq_n_f32(rr);

        while (p < pEnd8)
        {
            // load 4 samples for each channel, deinterleaving
            float32x4x2_t in = vld2q_f32(p);
            float32x4x2_t out;

            // left
            out.val[0] = vmulq_f32(in.val[0], llv);
            out.val[0] = vmlaq_f32(out.val[0], in.val[1], lrv);

            // right
            out.val[1] = vmulq_f32(in.val[0], rlv);
            out.val[1] = vmlaq_f32(out.val[1], in.val[1], rrv);

            vst2q_f32(p, out);

            p += 8;
        }

        while (p < pEnd)
        {
            float in_l = *p;
            float in_r = *(p+1);

            *p++ = in_l * ll + in_r * lr;
            *p++ = in_l * rl + in_r * rr;
        }
    }

    int stereoPanRamp_neon(int32* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        VolumeMatrix volDiff = targetVolume - volume;

        // frames must be multiple of 4
        int llFrames = Math::abs((int) (volDiff.ll / volumeRate)) & ~3;
        int lrFrames = Math::abs((int) (volDiff.lr / volumeRate)) & ~3;
        int rlFrames = Math::abs((int) (volDiff.rl / volumeRate)) & ~3;
        int rrFrames = Math::abs((int) (volDiff.rr / volumeRate)) & ~3;

        int rampFrames = Math::max(llFrames, lrFrames, rlFrames, rrFrames);
        rampFrames = Math::min(rampFrames, frames);

        int32 llInc = llFrames > 0 ? Fixed8_24::fromFloat(volDiff.ll / llFrames) : 0;
        int32 lrInc = lrFrames > 0 ? Fixed8_24::fromFloat(volDiff.lr / lrFrames) : 0;
        int32 rlInc = rlFrames > 0 ? Fixed8_24::fromFloat(volDiff.rl / rlFrames) : 0;
        int32 rrInc = rrFrames > 0 ? Fixed8_24::fromFloat(volDiff.rr / rrFrames) : 0;

        int32 ll = Fixed8_24::fromFloat(volume.ll);
        int32 lr = Fixed8_24::fromFloat(volume.lr);
        int32 rl = Fixed8_24::fromFloat(volume.rl);
        int32 rr = Fixed8_24::fromFloat(volume.rr);

        int32* p = buf;
        const int32* pEnd8 = buf + ((rampFrames * 2) & ~7);
        const int32* pEnd = buf + rampFrames * 2;

        // load constants
        int32 lla[] = { ll, ll + llInc };
        int32 lra[] = { lr, lr + lrInc };
        int32 rla[] = { rl, rl + rlInc };
        int32 rra[] = { rr, rr + rrInc };
        int32x2_t llv = vld1_s32(lla);
        int32x2_t lrv = vld1_s32(lra);
        int32x2_t rlv = vld1_s32(rla);
        int32x2_t rrv = vld1_s32(rra);
        int32x2_t llIncv = vdup_n_s32(llInc*2);
        int32x2_t lrIncv = vdup_n_s32(lrInc*2);
        int32x2_t rlIncv = vdup_n_s32(rlInc*2);
        int32x2_t rrIncv = vdup_n_s32(rrInc*2);

        int nFrames = 0;
        while (p < pEnd8)
        {
            // load 4 samples for each channel, deinterleaving
            int32x4x2_t in = vld2q_s32(p);

            int32x2_t in_left_low = vget_low_s32(in.val[0]);
            int32x2_t in_right_low = vget_low_s32(in.val[1]);
            int32x2_t in_left_high = vget_high_s32(in.val[0]);
            int32x2_t in_right_high = vget_high_s32(in.val[1]);

            int32x4x2_t out;
            int64x2_t tmp;

            // left low
            tmp = vmull_s32(in_left_low, llv);
            tmp = vmlal_s32(tmp, in_right_low, lrv);
            int32x2_t out_left_low = vshrn_n_s64(tmp, 24);

            // right low
            tmp = vmull_s32(in_left_low, rlv);
            tmp = vmlal_s32(tmp, in_right_low, rrv);
            int32x2_t out_right_low = vshrn_n_s64(tmp, 24);

            if (nFrames < llFrames) llv = vadd_s32(llv, llIncv);
            if (nFrames < lrFrames) lrv = vadd_s32(lrv, lrIncv);
            if (nFrames < rlFrames) rlv = vadd_s32(rlv, rlIncv);
            if (nFrames < rrFrames) rrv = vadd_s32(rrv, rrIncv);
            nFrames += 2;

            // left high
            tmp = vmull_s32(in_left_high, llv);
            tmp = vmlal_s32(tmp, in_right_high, lrv);
            int32x2_t out_left_high = vshrn_n_s64(tmp, 24);

            // right high
            tmp = vmull_s32(in_left_high, rlv);
            tmp = vmlal_s32(tmp, in_right_high, rrv);
            int32x2_t out_right_high = vshrn_n_s64(tmp, 24);

            if (nFrames < llFrames) llv = vadd_s32(llv, llIncv);
            if (nFrames < lrFrames) lrv = vadd_s32(lrv, lrIncv);
            if (nFrames < rlFrames) rlv = vadd_s32(rlv, rlIncv);
            if (nFrames < rrFrames) rrv = vadd_s32(rrv, rrIncv);
            nFrames += 2;

            out.val[0] = vcombine_s32(out_left_low, out_left_high);
            out.val[1] = vcombine_s32(out_right_low, out_right_high);

            vst2q_s32(p, out);

            p += 8;

        }

        ll = vget_lane_s32(llv, 0);
        lr = vget_lane_s32(lrv, 0);
        rl = vget_lane_s32(rlv, 0);
        rr = vget_lane_s32(rrv, 0);

        while (p < pEnd)
        {
            int64 in_l = *p;
            int64 in_r = *(p+1);

            *p++ = (int32) ((in_l * ll + in_r * lr) >> 24);
            *p++ = (int32) ((in_l * rl + in_r * rr) >> 24);

            if (nFrames < llFrames) ll += llInc;
            if (nFrames < lrFrames) lr += lrInc;
            if (nFrames < rlFrames) rl += rlInc;
            if (nFrames < rrFrames) rr += rrInc;
            ++nFrames;
        }

        // set output volume
        volume.ll = (nFrames >= llFrames ? targetVolume.ll : Fixed8_24::toFloat(ll));
        volume.lr = (nFrames >= lrFrames ? targetVolume.lr : Fixed8_24::toFloat(lr));
        volume.rl = (nFrames >= rlFrames ? targetVolume.rl : Fixed8_24::toFloat(rl));
        volume.rr = (nFrames >= rrFrames ? targetVolume.rr : Fixed8_24::toFloat(rr));

        return rampFrames;
    }

    int stereoPanRamp_neon(float* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        VolumeMatrix volDiff = targetVolume - volume;

        // frames must be multiple of 4
        int llFrames = Math::abs((int) (volDiff.ll / volumeRate)) & ~3;
        int lrFrames = Math::abs((int) (volDiff.lr / volumeRate)) & ~3;
        int rlFrames = Math::abs((int) (volDiff.rl / volumeRate)) & ~3;
        int rrFrames = Math::abs((int) (volDiff.rr / volumeRate)) & ~3;

        int rampFrames = Math::max(llFrames, lrFrames, rlFrames, rrFrames);
        rampFrames = Math::min(rampFrames, frames);

        float llInc = llFrames > 0 ? volDiff.ll / llFrames : 0;
        float lrInc = lrFrames > 0 ? volDiff.lr / lrFrames : 0;
        float rlInc = rlFrames > 0 ? volDiff.rl / rlFrames : 0;
        float rrInc = rrFrames > 0 ? volDiff.rr / rrFrames : 0;

        float ll = volume.ll;
        float lr = volume.lr;
        float rl = volume.rl;
        float rr = volume.rr;

        float* p = buf;
        const float* pEnd8 = buf + ((rampFrames * 2) & ~7);
        const float* pEnd = buf + rampFrames * 2;

        // load constants
        float lla[] = { ll, ll + llInc, ll + llInc*2, ll+llInc*3 };
        float lra[] = { lr, lr + lrInc, lr + lrInc*2, lr+lrInc*3 };
        float rla[] = { rl, rl + rlInc, rl + rlInc*2, rl+rlInc*3 };
        float rra[] = { rr, rr + rrInc, rr + rrInc*2, rr+rrInc*3 };
        float32x4_t llv = vld1q_f32(lla);
        float32x4_t lrv = vld1q_f32(lra);
        float32x4_t rlv = vld1q_f32(rla);
        float32x4_t rrv = vld1q_f32(rra);
        float32x4_t llIncv = vdupq_n_f32(llInc*4);
        float32x4_t lrIncv = vdupq_n_f32(lrInc*4);
        float32x4_t rlIncv = vdupq_n_f32(rlInc*4);
        float32x4_t rrIncv = vdupq_n_f32(rrInc*4);

        int nFrames = 0;
        while (p < pEnd8)
        {
            // load 4 samples for each channel, deinterleaving
            float32x4x2_t in = vld2q_f32(p);

            float32x4_t in_left = in.val[0];
            float32x4_t in_right = in.val[1];

            float32x4x2_t out;

            // left 
            out.val[0] = vmulq_f32(in_left, llv);
            out.val[0] = vmlaq_f32(out.val[0], in_right, lrv);

            // right 
            out.val[1] = vmulq_f32(in_left, rlv);
            out.val[1] = vmlaq_f32(out.val[1], in_right, rrv);

            if (nFrames < llFrames) llv = vaddq_f32(llv, llIncv);
            if (nFrames < lrFrames) lrv = vaddq_f32(lrv, lrIncv);
            if (nFrames < rlFrames) rlv = vaddq_f32(rlv, rlIncv);
            if (nFrames < rrFrames) rrv = vaddq_f32(rrv, rrIncv);
            nFrames += 4;

            vst2q_f32(p, out);

            p += 8;
        }

        ll = vgetq_lane_f32(llv, 0);
        lr = vgetq_lane_f32(lrv, 0);
        rl = vgetq_lane_f32(rlv, 0);
        rr = vgetq_lane_f32(rrv, 0);

        while (p < pEnd)
        {
            float in_l = *p;
            float in_r = *(p+1);

            *p++ = in_l * ll + in_r * lr;
            *p++ = in_l * rl + in_r * rr;

            if (nFrames < llFrames) ll += llInc;
            if (nFrames < lrFrames) lr += lrInc;
            if (nFrames < rlFrames) rl += rlInc;
            if (nFrames < rrFrames) rr += rrInc;
            ++nFrames;
        }

        // set output volume
        volume.ll = (nFrames >= llFrames ? targetVolume.ll : ll);
        volume.lr = (nFrames >= lrFrames ? targetVolume.lr : lr);
        volume.rl = (nFrames >= rlFrames ? targetVolume.rl : rl);
        volume.rr = (nFrames >= rrFrames ? targetVolume.rr : rr);

        return rampFrames;
    }

    ////////////////////////////////////////
    // mono pan

    void monoPan_neon(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& volume)
    {
        int32 l = Fixed8_24::fromFloat(volume.ll);
        int32 r = Fixed8_24::fromFloat(volume.rr);

        const int32* pIn = inBuf;
        const int32* pInEnd4 = inBuf + ((frames) & ~3);
        const int32* pInEnd = inBuf + frames;
        int32* pOut = outBuf;

        // load constants
        int32x2_t lv = vdup_n_s32(l);
        int32x2_t rv = vdup_n_s32(r);

        while (pIn < pInEnd4)
        {
            int32x4_t in = vld1q_s32(pIn);

            int32x2_t in_low = vget_low_s32(in);
            int32x2_t in_high = vget_high_s32(in);

            int32x4x2_t out;
            int32x2_t out_low;
            int32x2_t out_high;
            int64x2_t tmp;

            // left
            tmp = vmull_s32(in_low, lv);
            out_low = vshrn_n_s64(tmp, 24);

            tmp = vmull_s32(in_high, lv);
            out_high = vshrn_n_s64(tmp, 24);

            out.val[0] = vcombine_s32(out_low, out_high);

            // right
            tmp = vmull_s32(in_low, rv);
            out_low = vshrn_n_s64(tmp, 24);

            tmp = vmull_s32(in_high, rv);
            out_high = vshrn_n_s64(tmp, 24);

            out.val[1] = vcombine_s32(out_low, out_high);

            vst2q_s32(pOut, out);

            pOut += 8;
            pIn += 4;
        }

        while (pIn < pInEnd)
        {
            int64 in = *pIn++;

            *pOut++ = (int32) ((in * l) >> 24);
            *pOut++ = (int32) ((in * r) >> 24);
        }
    }

    void monoPan_neon(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& volume)
    {
        float l = volume.ll;
        float r = volume.rr;

        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + ((frames) & ~3);
        const float* pInEnd = inBuf + frames;
        float* pOut = outBuf;

        // load constants
        float32x4_t lv = vdupq_n_f32(l);
        float32x4_t rv = vdupq_n_f32(r);

        while (pIn < pInEnd4)
        {
            float32x4_t in = vld1q_f32(pIn);
            float32x4x2_t out;

            // left
            out.val[0] = vmulq_f32(in, lv);

            // right
            out.val[1] = vmulq_f32(in, rv);

            vst2q_f32(pOut, out);

            pOut += 8;
            pIn += 4;
        }

        while (pIn < pInEnd)
        {
            float in = *pIn++;

            *pOut++ = in * l;
            *pOut++ = in * r;
        }
    }

    int monoPanRamp_neon(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        float lDiff = targetVolume.ll - volume.ll;
        float rDiff = targetVolume.rr - volume.rr;

        // frames must be multiple of 4
        int lFrames = Math::abs((int) (lDiff / volumeRate)) & ~3;
        int rFrames = Math::abs((int) (rDiff / volumeRate)) & ~3;

        int rampFrames = Math::max(lFrames, rFrames);
        rampFrames = Math::min(rampFrames, frames);

        int32 lInc = lFrames > 0 ? Fixed8_24::fromFloat(lDiff / lFrames) : 0;
        int32 rInc = rFrames > 0 ? Fixed8_24::fromFloat(rDiff / rFrames) : 0;

        int32 l = Fixed8_24::fromFloat(volume.ll);
        int32 r = Fixed8_24::fromFloat(volume.rr);

        const int32* pIn = inBuf;
        const int32* pInEnd4 = inBuf + (rampFrames & ~3);
        const int32* pInEnd = inBuf + rampFrames;
        int32* pOut = outBuf;

        // load constants
        int32 la[] = { l, l + lInc };
        int32 ra[] = { r, r + rInc };
        int32x2_t lv = vld1_s32(la);
        int32x2_t rv = vld1_s32(ra);
        int32x2_t lIncv = vdup_n_s32(lInc*2);
        int32x2_t rIncv = vdup_n_s32(rInc*2);

        int nFrames = 0;
        while (pIn < pInEnd4)
        {
            int32x4_t in = vld1q_s32(pIn);

            int32x2_t in_low = vget_low_s32(in);
            int32x2_t in_high = vget_high_s32(in);

            int32x4x2_t out;
            int32x2_t out_left_low;
            int32x2_t out_left_high;
            int32x2_t out_right_low;
            int32x2_t out_right_high;
            int64x2_t tmp;

            // left low
            tmp = vmull_s32(in_low, lv);
            out_left_low = vshrn_n_s64(tmp, 24);

            // right low
            tmp = vmull_s32(in_low, rv);
            out_right_low = vshrn_n_s64(tmp, 24);

            if (nFrames < lFrames) lv = vadd_s32(lv, lIncv);
            if (nFrames < rFrames) rv = vadd_s32(rv, rIncv);
            nFrames += 2;

            // left high
            tmp = vmull_s32(in_high, lv);
            out_left_high = vshrn_n_s64(tmp, 24);

            // right high
            tmp = vmull_s32(in_high, rv);
            out_right_high = vshrn_n_s64(tmp, 24);

            out.val[0] = vcombine_s32(out_left_low, out_left_high);
            out.val[1] = vcombine_s32(out_right_low, out_right_high);

            if (nFrames < lFrames) lv = vadd_s32(lv, lIncv);
            if (nFrames < rFrames) rv = vadd_s32(rv, rIncv);
            nFrames += 2;

            vst2q_s32(pOut, out);

            pOut += 8;
            pIn += 4;
        }

        l = vget_lane_s32(lv, 0);
        r = vget_lane_s32(rv, 0);

        while (pIn < pInEnd)
        {
            int64 in = *pIn++;

            *pOut++ = (int32) ((in * l) >> 24);
            *pOut++ = (int32) ((in * r) >> 24);

            if (nFrames < lFrames) l += lInc;
            if (nFrames < rFrames) r += rInc;
            ++nFrames;
        }

        // set output volume
        volume.ll = (nFrames >= lFrames ? targetVolume.ll : Fixed8_24::toFloat(l));
        volume.rr = (nFrames >= rFrames ? targetVolume.rr : Fixed8_24::toFloat(r));

        // these are ignored for mono input, but ramp them all the way
        volume.lr = targetVolume.lr;
        volume.rl = targetVolume.rl;

        return rampFrames;
    }

    int monoPanRamp_neon(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        float lDiff = targetVolume.ll - volume.ll;
        float rDiff = targetVolume.rr - volume.rr;

        // frames must be multiple of 4
        int lFrames = Math::abs((int) (lDiff / volumeRate)) & ~3;
        int rFrames = Math::abs((int) (rDiff / volumeRate)) & ~3;

        int rampFrames = Math::max(lFrames, rFrames);
        rampFrames = Math::min(rampFrames, frames);

        float lInc = lFrames > 0 ? lDiff / lFrames : 0;
        float rInc = rFrames > 0 ? rDiff / rFrames : 0;

        float l = volume.ll;
        float r = volume.rr;

        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (rampFrames & ~3);
        const float* pInEnd = inBuf + rampFrames;
        float* pOut = outBuf;

        // load constants
        float la[] = { l, l + lInc, l + lInc*2, l+lInc*3 };
        float ra[] = { r, r + rInc, r + rInc*2, r+rInc*3 };
        float32x4_t lv = vld1q_f32(la);
        float32x4_t rv = vld1q_f32(ra);
        float32x4_t lIncv = vdupq_n_f32(lInc*4);
        float32x4_t rIncv = vdupq_n_f32(rInc*4);

        int nFrames = 0;
        while (pIn < pInEnd4)
        {
            float32x4_t in = vld1q_f32(pIn);

            float32x4x2_t out;
            out.val[0] = vmulq_f32(in, lv);
            out.val[1] = vmulq_f32(in, rv);

            if (nFrames < lFrames) lv = vaddq_f32(lv, lIncv);
            if (nFrames < rFrames) rv = vaddq_f32(rv, rIncv);
            nFrames += 4;

            vst2q_f32(pOut, out);

            pOut += 8;
            pIn += 4;
        }

        l = vgetq_lane_f32(lv, 0);
        r = vgetq_lane_f32(rv, 0);

        while (pIn < pInEnd)
        {
            float in = *pIn++;

            *pOut++ = in * l;
            *pOut++ = in * r;

            if (nFrames < lFrames) l += lInc;
            if (nFrames < rFrames) r += rInc;
            ++nFrames;
        }

        // set output volume
        volume.ll = (nFrames >= lFrames ? targetVolume.ll : l);
        volume.rr = (nFrames >= rFrames ? targetVolume.rr : r);

        // these are ignored for mono input, but ramp them all the way
        volume.lr = targetVolume.lr;
        volume.rl = targetVolume.rl;

        return rampFrames;
    }

    ////////////////////////////////////////
    // resample

    void resampleMono_neon(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput)
    {
        int32* pOut = outBuf;
        const int32* pOutEnd4 = outBuf + outFrames - 4;
        const int32* pOutEnd = outBuf + outFrames;

        float factor = (float) inFrames / outFrames;
        uint32 factor_16_16 = Fixed16_16::fromFloat(factor);

        int32 sample0;
        int32 sample1;

        int t0 = 0;
        uint32 t_16_16 = 0;

        // interpolate between last sample of previous slice and first sample of current slice
        sample0 = *lastInput;
        sample1 = *inBuf;
        while (t0 == 0)
        {
            *pOut++ = sample0 + (int32) (((int64) (t_16_16 & 0x0000ffff) * (int64) (sample1 - sample0)) >> 16);

            t_16_16 += factor_16_16;
            t0 = t_16_16 >> 16;
        }

        // resample the rest of the current slice
        int32x4_t tv = {0};
        tv = vsetq_lane_s32(t_16_16, tv, 0);
        tv = vsetq_lane_s32(t_16_16 + factor_16_16, tv, 1);
        tv = vsetq_lane_s32(t_16_16 + 2*factor_16_16, tv, 2);
        tv = vsetq_lane_s32(t_16_16 + 3*factor_16_16, tv, 3);

        int32x4_t factorv = vdupq_n_s32(4*factor_16_16);
        int32x4_t maskv = vdupq_n_s32(0x0000ffff);

        const int32* pIn = inBuf + (t0-1);
        int prevt0 = t0;
		int32x4_t sample0v = {0};
		int32x4_t sample1v = {0};
        while (pOut < pOutEnd4)
        {
            sample0v = vld1q_lane_s32(pIn, sample0v, 0);
            sample1v = vld1q_lane_s32(pIn+1, sample1v, 0);
            t_16_16 += factor_16_16;
            t0 = t_16_16 >> 16;
            pIn += (t0 - prevt0);
            prevt0 = t0;

            sample0v = vld1q_lane_s32(pIn, sample0v, 1);
            sample1v = vld1q_lane_s32(pIn+1, sample1v, 1);
            t_16_16 += factor_16_16;
            t0 = t_16_16 >> 16;
            pIn += (t0 - prevt0);
            prevt0 = t0;

            sample0v = vld1q_lane_s32(pIn, sample0v, 2);
            sample1v = vld1q_lane_s32(pIn+1, sample1v, 2);
            t_16_16 += factor_16_16;
            t0 = t_16_16 >> 16;
            pIn += (t0 - prevt0);
            prevt0 = t0;

            sample0v = vld1q_lane_s32(pIn, sample0v, 3);
            sample1v = vld1q_lane_s32(pIn+1, sample1v, 3);
            t_16_16 += factor_16_16;
            t0 = t_16_16 >> 16;
            pIn += (t0 - prevt0);
            prevt0 = t0;

            int32x4_t frac = vandq_s32(tv, maskv);
            int32x2_t frac_low = vget_low_s32(frac);
            int32x2_t frac_high = vget_high_s32(frac);

            int32x4_t diff;
            int32x2_t diff2;
            int64x2_t tmp;
            int32x2_t tmp2_low;
            int32x2_t tmp2_high;
            int32x4_t out;

            diff = vsubq_s32(sample1v, sample0v);

            diff2 = vget_low_s32(diff);
            tmp = vmull_s32(frac_low, diff2);
            tmp2_low = vshrn_n_s64(tmp, 16);

            diff2 = vget_high_s32(diff);
            tmp = vmull_s32(frac_high, diff2);
            tmp2_high = vshrn_n_s64(tmp, 16);

            out = vcombine_s32(tmp2_low, tmp2_high);
            out = vaddq_s32(out, sample0v);

            vst1q_s32(pOut, out);
            pOut += 4;

            tv = vaddq_s32(tv, factorv);
        }

        while (pOut < pOutEnd)
        {
            sample0 = *pIn;
            sample1 = *(pIn+1);

            *pOut++ = sample0 + (int32) (((int64) (t_16_16 & 0x0000ffff) * (int64) (sample1 - sample0)) >> 16);

            t_16_16 += factor_16_16;
            t0 = t_16_16 >> 16;

            pIn += (t0 - prevt0);
            prevt0 = t0;
        }

        *lastInput = *(inBuf + inFrames - 1);
    }

    void resampleStereo_neon(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput)
    {
        int32* pOut = outBuf;
        const int32* pOutEnd8 = outBuf + outFrames*2 - 8;
        const int32* pOutEnd = outBuf + outFrames*2;

        float factor = (float) inFrames / outFrames;
        uint32 factor_16_16 = Fixed16_16::fromFloat(factor);

        int32 sample0L;
        int32 sample0R;
        int32 sample1L;
        int32 sample1R;

        int t0 = 0;
        uint32 t_16_16 = 0;

        // interpolate between last sample of previous slice and first sample of current slice
        sample0L = *(lastInput+0);
        sample0R = *(lastInput+1);
        sample1L = *(inBuf+0);
        sample1R = *(inBuf+1);
        while (t0 == 0)
        {
            uint32 frac_16_16 = (t_16_16 & 0x0000ffff);
            *pOut++ = sample0L + (int32) (((int64) frac_16_16 * (int64) (sample1L - sample0L)) >> 16);
            *pOut++ = sample0R + (int32) (((int64) frac_16_16 * (int64) (sample1R - sample0R)) >> 16);

            t_16_16 += factor_16_16;
            t0 = t_16_16 >> 16;
        }

        // resample the rest of the current slice
        int32x4_t tv = {0};
        tv = vsetq_lane_s32(t_16_16, tv, 0);
        tv = vsetq_lane_s32(t_16_16 + factor_16_16, tv, 1);
        tv = vsetq_lane_s32(t_16_16 + 2*factor_16_16, tv, 2);
        tv = vsetq_lane_s32(t_16_16 + 3*factor_16_16, tv, 3);

        int32x4_t factorv = vdupq_n_s32(4*factor_16_16);
        int32x4_t maskv = vdupq_n_s32(0x0000ffff);

        const int32* pIn = inBuf + 2*(t0-1);
        int prevt0 = t0;
		int32x4x2_t sample0v = {{0}};
		int32x4x2_t sample1v = {{0}};
        while (pOut < pOutEnd8)
        {
            sample0v = vld2q_lane_s32(pIn, sample0v, 0);
            sample1v = vld2q_lane_s32(pIn+2, sample1v, 0);
            t_16_16 += factor_16_16;
            t0 = t_16_16 >> 16;
            pIn += (t0 - prevt0)*2;
            prevt0 = t0;

            sample0v = vld2q_lane_s32(pIn, sample0v, 1);
            sample1v = vld2q_lane_s32(pIn+2, sample1v, 1);
            t_16_16 += factor_16_16;
            t0 = t_16_16 >> 16;
            pIn += (t0 - prevt0)*2;
            prevt0 = t0;

            sample0v = vld2q_lane_s32(pIn, sample0v, 2);
            sample1v = vld2q_lane_s32(pIn+2, sample1v, 2);
            t_16_16 += factor_16_16;
            t0 = t_16_16 >> 16;
            pIn += (t0 - prevt0)*2;
            prevt0 = t0;

            sample0v = vld2q_lane_s32(pIn, sample0v, 3);
            sample1v = vld2q_lane_s32(pIn+2, sample1v, 3);
            t_16_16 += factor_16_16;
            t0 = t_16_16 >> 16;
            pIn += (t0 - prevt0)*2;
            prevt0 = t0;

            int32x4_t frac = vandq_s32(tv, maskv);
            int32x2_t frac_low = vget_low_s32(frac);
            int32x2_t frac_high = vget_high_s32(frac);

            int32x4_t diff;
            int32x2_t diff2;
            int64x2_t tmp;
            int32x2_t tmp2_low;
            int32x2_t tmp2_high;
            int32x4x2_t out;


            diff = vsubq_s32(sample1v.val[0], sample0v.val[0]);

            diff2 = vget_low_s32(diff);
            tmp = vmull_s32(frac_low, diff2);
            tmp2_low = vshrn_n_s64(tmp, 16);

            diff2 = vget_high_s32(diff);
            tmp = vmull_s32(frac_high, diff2);
            tmp2_high = vshrn_n_s64(tmp, 16);

            out.val[0] = vcombine_s32(tmp2_low, tmp2_high);
            out.val[0] = vaddq_s32(out.val[0], sample0v.val[0]);


            diff = vsubq_s32(sample1v.val[1], sample0v.val[1]);

            diff2 = vget_low_s32(diff);
            tmp = vmull_s32(frac_low, diff2);
            tmp2_low = vshrn_n_s64(tmp, 16);

            diff2 = vget_high_s32(diff);
            tmp = vmull_s32(frac_high, diff2);
            tmp2_high = vshrn_n_s64(tmp, 16);

            out.val[1] = vcombine_s32(tmp2_low, tmp2_high);
            out.val[1] = vaddq_s32(out.val[1], sample0v.val[1]);

            vst2q_s32(pOut, out);
            pOut += 8;

            tv = vaddq_s32(tv, factorv);
        }

        while (pOut < pOutEnd)
        {
            sample0L = *pIn;
            sample0R = *(pIn+1);
            sample1L = *(pIn+2);
            sample1R = *(pIn+3);

            uint32 frac_16_16 = (t_16_16 & 0x0000ffff);
            *pOut++ = sample0L + (int32) (((int64) frac_16_16 * (int64) (sample1L - sample0L)) >> 16);
            *pOut++ = sample0R + (int32) (((int64) frac_16_16 * (int64) (sample1R - sample0R)) >> 16);

            t_16_16 += factor_16_16;
            t0 = t_16_16 >> 16;

            pIn += (t0 - prevt0)*2;
            prevt0 = t0;
        }

        lastInput[0] = *(inBuf + inFrames*2 - 2);
        lastInput[1] = *(inBuf + inFrames*2 - 1);
    }

    void resampleMono_neon(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput)
    {
        float* pOut = outBuf;
        const float* pOutEnd4 = outBuf + outFrames - 4;
        const float* pOutEnd = outBuf + outFrames;

        float factor = (float) inFrames / outFrames;

        float ti = 0.0f; // input frame
        int ti0 = 0;     // whole number part of input frame
        float to = 0.0f; // output frame
        float frac = 0.0f;

        // interpolate between last sample of previous slice and first sample of current slice
        float sample0 = *lastInput;
        float sample1 = *inBuf;
        while (ti0 == 0)
        {
            *pOut++ = sample0 + frac * (sample1 - sample0);

            ti = ++to * factor;
            ti0 = (int) ti;
            frac = ti - ti0;
        }

        // resample the rest of the current slice
        float toa[] = { to, to+1, to+2, to+3 };
        float32x4_t tov = vld1q_f32(toa);
        float32x4_t dtov = vdupq_n_f32(4.0f);
        float32x4_t factorv = vdupq_n_f32(factor);
        float32x4_t fracv;
		float32x4_t sample0v = { 0 };
		float32x4_t sample1v = { 0 };
		float32x4_t tiv;
		float32x4_t ti0v;
		int32x4_t ti0vi;
		float32x4_t out;

        while (pOut < pOutEnd4)
        {
            tiv = vmulq_f32(tov, factorv);
            ti0vi = vcvtq_s32_f32(tiv);
            ti0v = vcvtq_f32_s32(ti0vi);
            fracv = vsubq_f32(tiv, ti0v);

            // TODO load and zip/unzip
            ti0 = vgetq_lane_s32(ti0vi, 0);
            sample0v = vsetq_lane_f32(inBuf[ti0-1], sample0v, 0);
            sample1v = vsetq_lane_f32(inBuf[ti0], sample1v, 0);

            ti0 = vgetq_lane_s32(ti0vi, 1);
            sample0v = vsetq_lane_f32(inBuf[ti0-1], sample0v, 1);
            sample1v = vsetq_lane_f32(inBuf[ti0], sample1v, 1);

            ti0 = vgetq_lane_s32(ti0vi, 2);
            sample0v = vsetq_lane_f32(inBuf[ti0-1], sample0v, 2);
            sample1v = vsetq_lane_f32(inBuf[ti0], sample1v, 2);

            ti0 = vgetq_lane_s32(ti0vi, 3);
            sample0v = vsetq_lane_f32(inBuf[ti0-1], sample0v, 3);
            sample1v = vsetq_lane_f32(inBuf[ti0], sample1v, 3);

            out = vsubq_f32(sample1v, sample0v);
            out = vmlaq_f32(sample0v, fracv, out);

            vst1q_f32(pOut, out);
            pOut += 4;

            tov = vaddq_f32(tov, dtov);
        }

        to = vgetq_lane_f32(tov, 0);
        ti = to * factor;
        ti0 = (int) ti;
        frac = ti - ti0;

        while (pOut < pOutEnd)
        {
            sample0 = inBuf[ti0-1];
            sample1 = inBuf[ti0];

            *pOut++ = sample0 + frac * (sample1 - sample0);

            ti = ++to * factor;
            ti0 = (int) ti;
            frac = ti - ti0;
        }

        lastInput[0] = inBuf[inFrames - 1];
    }

    void resampleStereo_neon(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput)
    {
        float* pOut = outBuf;
        const float* pOutEnd8 = outBuf + outFrames*2 - 8;
        const float* pOutEnd = outBuf + outFrames*2;

        float factor = (float) inFrames / outFrames;

        float ti = 0.0f; // input frame
        int ti0 = 0;     // whole number part of input frame
        float to = 0.0f; // output frame
        float frac = 0.0f;

        // interpolate between last sample of previous slice and first sample of current slice
        float sample0L = lastInput[0];
        float sample0R = lastInput[1];
        float sample1L = inBuf[0];
        float sample1R = inBuf[1];
        while (ti0 == 0)
        {
            *pOut++ = sample0L + frac * (sample1L - sample0L);
            *pOut++ = sample0R + frac * (sample1R - sample0R);

            ti = ++to * factor;
            ti0 = (int) ti;
            frac = ti - ti0;
        }

        // resample the rest of the current slice
        float toa[] = { to, to+1, to+2, to+3 };
        float32x4_t tov = vld1q_f32(toa);
        float32x4_t dtov = vdupq_n_f32(4.0f);
        float32x4_t factorv = vdupq_n_f32(factor);
        float32x4_t fracv;
		float32x4x2_t sample0v;
		float32x4x2_t sample1v;
		float32x4_t tiv;
		float32x4_t ti0v;
		int32x4_t ti0vi;
		float32x4x2_t out;

        while (pOut < pOutEnd8)
        {
            tiv = vmulq_f32(tov, factorv);
            ti0vi = vcvtq_s32_f32(tiv);
            ti0v = vcvtq_f32_s32(ti0vi);
            fracv = vsubq_f32(tiv, ti0v);

            float32x4_t in0 = vld1q_f32(inBuf + (vgetq_lane_s32(ti0vi, 0) - 1)*2);
            float32x4_t in1 = vld1q_f32(inBuf + (vgetq_lane_s32(ti0vi, 1) - 1)*2);
            float32x4_t in2 = vld1q_f32(inBuf + (vgetq_lane_s32(ti0vi, 2) - 1)*2);
            float32x4_t in3 = vld1q_f32(inBuf + (vgetq_lane_s32(ti0vi, 3) - 1)*2);

            sample0v = vzipq_f32(in0, in1);
            sample1v = vzipq_f32(in2, in3);

            float32x2_t x00l = vget_low_f32(sample0v.val[0]);
            float32x2_t x00h = vget_high_f32(sample0v.val[0]);
            float32x2_t x01l = vget_low_f32(sample0v.val[1]);
            float32x2_t x01h = vget_high_f32(sample0v.val[1]);
            float32x2_t x10l = vget_low_f32(sample1v.val[0]);
            float32x2_t x10h = vget_high_f32(sample1v.val[0]);
            float32x2_t x11l = vget_low_f32(sample1v.val[1]);
            float32x2_t x11h = vget_high_f32(sample1v.val[1]);

            sample0v.val[0] = vcombine_f32(x00l, x10l);
            sample0v.val[1] = vcombine_f32(x00h, x10h);
            sample1v.val[0] = vcombine_f32(x01l, x11l);
            sample1v.val[1] = vcombine_f32(x01h, x11h);

            out.val[0] = vsubq_f32(sample1v.val[0], sample0v.val[0]);
            out.val[0] = vmlaq_f32(sample0v.val[0], fracv, out.val[0]);
            out.val[1] = vsubq_f32(sample1v.val[1], sample0v.val[1]);
            out.val[1] = vmlaq_f32(sample0v.val[1], fracv, out.val[1]);

            vst2q_f32(pOut, out);
            pOut += 8;

            tov = vaddq_f32(tov, dtov);
        }

        to = vgetq_lane_f32(tov, 0);
        ti = to * factor;
        ti0 = (int) ti;
        frac = ti - ti0;

        while (pOut < pOutEnd)
        {
            const float* pIn = inBuf + (ti0-1)*2;
            sample0L = *pIn++;
            sample0R = *pIn++;
            sample1L = *pIn++;
            sample1R = *pIn;

            *pOut++ = sample0L + frac * (sample1L - sample0L);
            *pOut++ = sample0R + frac * (sample1R - sample0R);

            ti = ++to * factor;
            ti0 = (int) ti;
            frac = ti - ti0;
        }

        lastInput[0] = inBuf[inFrames*2 - 2];
        lastInput[1] = inBuf[inFrames*2 - 1];
    }


    ////////////////////////////////////////
    // clamp 

    bool clamp_neon(const int32* inBuf, int32* outBuf, int samples)
    {
        const int32 k_min = ((uint32)-1 << 24) + 1;
        const int32 k_max = (1<<24) - 1;
        int32x4_t minv = vdupq_n_s32(k_min);
        int32x4_t maxv = vdupq_n_s32(k_max);
        uint32x4_t clipFinal = { 0 };

        const int32* pIn = inBuf;
        const int32* pInEnd8 = inBuf + (samples & ~7);
        const int32* pInEnd = inBuf + samples;
        int32* pOut = outBuf;
        while (pIn < pInEnd8)
        {
            int32x4_t in = vld1q_s32(pIn);

            uint32x4_t clip;
            clip = vcgtq_s32(in, maxv);
            clipFinal = vorrq_u32(clipFinal, clip);
            clip = vcltq_s32(in, minv);
            clipFinal = vorrq_u32(clipFinal, clip);

            in = vminq_s32(in, maxv);
            in = vmaxq_s32(in, minv);

            vst1q_s32(pOut, in);

            pIn += 4;
            pOut += 4;
        }

        bool clip = false;
        while (pIn < pInEnd)
        {
            int32 val = *pIn++;

            if (val > k_max)
            {
                *pOut = k_max;
                clip = true;
            }
            else if (val < k_min)
            {
                *pOut = k_min;
                clip = true;
            }
            else
            {
                *pOut = val;
            }

            ++pOut;
        }

        if (!clip)
        {
            uint32 c = 0;
            c |= vgetq_lane_u32(clipFinal, 0);
            c |= vgetq_lane_u32(clipFinal, 1);
            c |= vgetq_lane_u32(clipFinal, 2);
            c |= vgetq_lane_u32(clipFinal, 3);
            clip = clip || (c != 0);
        }

        return clip;
    }

    bool clamp_neon(const float* inBuf, float* outBuf, int samples)
    {
        float32x4_t minv = vdupq_n_f32(-1.0f);
        float32x4_t maxv = vdupq_n_f32(1.0f);
        uint32x4_t clipFinal = { 0 };

        const float* pIn = inBuf;
        const float* pInEnd8 = inBuf + (samples & ~7);
        const float* pInEnd = inBuf + samples;
        float* pOut = outBuf;
        while (pIn < pInEnd8)
        {
            float32x4_t in = vld1q_f32(pIn);

            uint32x4_t clip;
            clip = vcgtq_f32(in, maxv);
            clipFinal = vorrq_u32(clipFinal, clip);
            clip = vcltq_f32(in, minv);
            clipFinal = vorrq_u32(clipFinal, clip);

            in = vminq_f32(in, maxv);
            in = vmaxq_f32(in, minv);

            vst1q_f32(pOut, in);

            pIn += 4;
            pOut += 4;
        }

        bool clip = false;
        while (pIn < pInEnd)
        {
            float val = *pIn++;

            if (val > 1.0f)
            {
                *pOut = 1.0f;
                clip = true;
            }
            else if (val < -1.0f)
            {
                *pOut = -1.0f;
                clip = true;
            }
            else
            {
                *pOut = val;
            }

            ++pOut;
        }

        if (!clip)
        {
            uint32 c = 0;
            c |= vgetq_lane_u32(clipFinal, 0);
            c |= vgetq_lane_u32(clipFinal, 1);
            c |= vgetq_lane_u32(clipFinal, 2);
            c |= vgetq_lane_u32(clipFinal, 3);
            clip = clip || (c != 0);
        }

        return clip;
    }

}

}
#endif // CK_ARM_NEON

