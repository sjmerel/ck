#include "ck/audio/audioutil.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/fixedpoint.h"
#include "ck/core/sse.h"
#include "ck/audio/volumematrix.h"

#if CK_X86_SSE

#include <emmintrin.h>

// NOTE: SSSE3 is supported on all 64-bit Intel Macs, but 32-bit Intel Macs
// only support SSE2.
//
// Fixed-point pipeline will almost never be used on Intel, so SSE optimizations
// of those functions have not been done except where it's really easy.

namespace Cki
{


namespace AudioUtil
{
    ////////////////////////////////////////
    // convert float to int32

    void convertF32toI32_sse(const float* inBuf, int32* outBuf, int samples)
    {
        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (samples & ~3);
        const float* pInEnd = inBuf + samples;
        int32* pOut = outBuf;

        const __m128 n = _mm_set1_ps((float) (1 << 24));

        while (pIn < pInEnd4)
        {
            __m128 in = _mm_loadu_ps(pIn);
            in = _mm_mul_ps(in, n);
            __m128i out = _mm_cvtps_epi32(in);
            _mm_storeu_si128((__m128i*) pOut, out);
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

    void convertF32toI16_sse(const float* inBuf, int16* outBuf, int samples)
    {
        const float* pIn = inBuf;
        const float* pInEnd8 = inBuf + (samples & ~7);
        const float* pInEnd = inBuf + samples;
        int16* pOut = outBuf;

        const __m128 n = _mm_set1_ps((float) (CK_INT16_MAX));

        while (pIn < pInEnd8)
        {
            __m128 in;

            in = _mm_loadu_ps(pIn);
            in = _mm_mul_ps(in, n);
            __m128i out0 = _mm_cvttps_epi32(in);
            pIn += 4;

            in = _mm_loadu_ps(pIn);
            in = _mm_mul_ps(in, n);
            __m128i out1 = _mm_cvttps_epi32(in);
            pIn += 4;

            __m128i out = _mm_packs_epi32(out0, out1);
            _mm_storeu_si128((__m128i*) pOut, out);
            pOut += 8;
        }

        while (pIn < pInEnd)
        {
            *pOut++ = (int16) (*pIn++ * CK_INT16_MAX);
        }
    }


    ////////////////////////////////////////
    // convert int32 to float

    void convertI32toF32_sse(const int32* inBuf, float* outBuf, int samples)
    {
        const int32* pIn = inBuf;
        const int32* pInEnd4 = inBuf + (samples & ~3);
        const int32* pInEnd = inBuf + samples;
        float* pOut = outBuf;

        const float factor = 1.0f / (1 << 24);
        const __m128 n = _mm_set1_ps(factor);

        while (pIn < pInEnd4)
        {
            __m128i in = _mm_loadu_si128((__m128i*) pIn);
            __m128 out = _mm_cvtepi32_ps(in);
            out = _mm_mul_ps(out, n);
            _mm_storeu_ps(pOut, out);
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

    void convertI32toI16_sse(const int32* inBuf, int16* outBuf, int samples)
    {
        convertI32toI16_default(inBuf, outBuf, samples);
    }

    ////////////////////////////////////////
    // convert int16 to float

    void convertI16toF32_sse(const int16* inBuf, float* outBuf, int samples)
    {
        // iterate backwards so we can do it in-place

        const int16* pIn = inBuf + samples - 1;
        const int16* pInEnd8 = inBuf + (samples & ~7) - 1;
        float* pOut = outBuf + samples - 1;
        const float factor = 1.0f / CK_INT16_MAX;

        while (pIn > pInEnd8)
        {
            *pOut-- = ((float) *pIn--) * factor;
        }
        pIn -= 7;
        pOut -= 3;

        const __m128 factorv = _mm_set1_ps(factor);
        const __m128i zero = _mm_setzero_si128();
        while (pIn >= inBuf)
        {
            __m128i in = _mm_loadu_si128((__m128i*) pIn);

            __m128i inh = _mm_unpackhi_epi16(in, zero);
            __m128i inl = _mm_unpacklo_epi16(in, zero);

            // shift left then right to fix signs
            inh = _mm_slli_epi32(inh, 16);
            inh = _mm_srai_epi32(inh, 16);
            inl = _mm_slli_epi32(inl, 16);
            inl = _mm_srai_epi32(inl, 16);

            __m128 out;
            
            out = _mm_cvtepi32_ps(inh);
            out = _mm_mul_ps(out, factorv);
            _mm_storeu_ps(pOut, out);
            pOut -= 4;

            out = _mm_cvtepi32_ps(inl);
            out = _mm_mul_ps(out, factorv);
            _mm_storeu_ps(pOut, out);
            pOut -= 4;

            pIn -= 8;
        }

        CK_ASSERT(pIn == inBuf - 8);
        CK_ASSERT(pOut == outBuf - 4);
    }



    ////////////////////////////////////////
    // convert int16 to int32

    void convertI16toI32_sse(const int16* inBuf, int32* outBuf, int samples)
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

        const __m128i shift = _mm_set1_epi16(1 << 9);

        while (pIn >= inBuf)
        {
            __m128i in = _mm_loadu_si128((__m128i*) pIn);

            __m128i outh = _mm_mulhi_epi16(in, shift);
            __m128i outl = _mm_mullo_epi16(in, shift);

            __m128i out;
            
            out = _mm_unpackhi_epi16(outl, outh);
            _mm_storeu_si128((__m128i*) pOut, out);
            pOut -= 4;

            out = _mm_unpacklo_epi16(outl, outh);
            _mm_storeu_si128((__m128i*) pOut, out);
            pOut -= 4;

            pIn -= 8;
        }

        CK_ASSERT(pIn == inBuf - 8);
        CK_ASSERT(pOut == outBuf - 4);
    }

    ////////////////////////////////////////
    // convert int8 to float

    void convertI8toF32_sse(const int8* inBuf, float* outBuf, int samples)
    {
        // iterate backwards so we can do it in-place

        const int8* pIn = inBuf + samples - 1;
        const int8* pInEnd16 = inBuf + (samples & ~15) - 1;
        float* pOut = outBuf + samples - 1;
        const float factor = 1.0f / CK_INT8_MAX;

        while (pIn > pInEnd16)
        {
            *pOut-- = ((float) *pIn--) * factor;
        }
        pIn -= 15;
        pOut -= 3;

        const __m128 factorv = _mm_set1_ps(factor);
        const __m128i zero = _mm_setzero_si128();
        while (pIn >= inBuf)
        {
            __m128i in = _mm_loadu_si128((__m128i*) pIn);

            __m128i in16h = _mm_unpackhi_epi8(in, zero);
            __m128i in16l = _mm_unpacklo_epi8(in, zero);

            __m128i in32hh = _mm_unpackhi_epi16(in16h, zero);
            __m128i in32hl = _mm_unpacklo_epi16(in16h, zero);
            __m128i in32lh = _mm_unpackhi_epi16(in16l, zero);
            __m128i in32ll = _mm_unpacklo_epi16(in16l, zero);

            in32hh = _mm_slli_epi32(in32hh, 24);
            in32hh = _mm_srai_epi32(in32hh, 24);
            in32hl = _mm_slli_epi32(in32hl, 24);
            in32hl = _mm_srai_epi32(in32hl, 24);
            in32lh = _mm_slli_epi32(in32lh, 24);
            in32lh = _mm_srai_epi32(in32lh, 24);
            in32ll = _mm_slli_epi32(in32ll, 24);
            in32ll = _mm_srai_epi32(in32ll, 24);

            __m128 out;
            
            out = _mm_cvtepi32_ps(in32hh);
            out = _mm_mul_ps(out, factorv);
            _mm_storeu_ps(pOut, out);
            pOut -= 4;

            out = _mm_cvtepi32_ps(in32hl);
            out = _mm_mul_ps(out, factorv);
            _mm_storeu_ps(pOut, out);
            pOut -= 4;

            out = _mm_cvtepi32_ps(in32lh);
            out = _mm_mul_ps(out, factorv);
            _mm_storeu_ps(pOut, out);
            pOut -= 4;

            out = _mm_cvtepi32_ps(in32ll);
            out = _mm_mul_ps(out, factorv);
            _mm_storeu_ps(pOut, out);
            pOut -= 4;

            pIn -= 16;
        }

        CK_ASSERT(pIn == inBuf - 16);
        CK_ASSERT(pOut == outBuf - 4);
    }


    ////////////////////////////////////////
    // convert int8 to int32

    void convertI8toI32_sse(const int8* inBuf, int32* outBuf, int samples)
    {
        convertI8toI32_default(inBuf, outBuf, samples);
    }


    ////////////////////////////////////////
    // mix

    void mix_sse(const int32* __restrict inBuf, int32* __restrict outBuf, int samples)
    {
        CK_ASSERT(inBuf != outBuf);

        const int32* pIn = inBuf;
        const int32* pInEnd4 = inBuf + (samples & ~3);
        const int32* pInEnd = inBuf + samples;
        int32* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            __m128i in = _mm_loadu_si128((__m128i*) pIn);
            __m128i out = _mm_loadu_si128((__m128i*) pOut);
            out = _mm_add_epi32(in, out);
            _mm_storeu_si128((__m128i*) pOut, out);

            pIn += 4;
            pOut += 4;
        }

        while (pIn < pInEnd)
        {
            *pOut++ += *pIn++;
        }
    }

    void mix_sse(const float* __restrict inBuf, float* __restrict outBuf, int samples)
    {
        CK_ASSERT(inBuf != outBuf);

        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (samples & ~3);
        const float* pInEnd = inBuf + samples;
        float* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            __m128 in = _mm_loadu_ps(pIn);
            __m128 out = _mm_loadu_ps(pOut);
            out = _mm_add_ps(in, out);
            _mm_storeu_ps(pOut, out);

            pIn += 4;
            pOut += 4;
        }

        while (pIn < pInEnd)
        {
            *pOut++ += *pIn++;
        }
    }

    void mixVol_sse(const int32* __restrict inBuf, float inVol, int32* __restrict outBuf, float outVol, int samples)
    {
        mixVol_default(inBuf, inVol, outBuf, outVol, samples);
    }

    void mixVol_sse(const float* __restrict inBuf, float inVol, float* __restrict outBuf, float outVol, int samples)
    {
        CK_ASSERT(inBuf != outBuf);

        __m128 outVolv = _mm_set1_ps(outVol);
        __m128 inVolv = _mm_set1_ps(inVol);

        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (samples & ~3);
        const float* pInEnd = inBuf + samples;
        float* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            __m128 in = _mm_loadu_ps(pIn);
            __m128 out = _mm_loadu_ps(pOut);
            in = _mm_mul_ps(in, inVolv);
            out = _mm_mul_ps(out, outVolv);
            out = _mm_add_ps(in, out);
            _mm_storeu_ps(pOut, out);
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

    void convertToStereo_sse(const int32* inBuf, int32* outBuf, int frames)
    {
        CK_ASSERT(inBuf != outBuf);
        const int32* pIn = inBuf;
        const int32* pInEnd4 = inBuf + (frames & ~3);
        const int32* pInEnd = inBuf + frames;
        int32* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            __m128i in = _mm_loadu_si128((__m128i*) pIn);
            __m128i out_low = _mm_unpacklo_epi32(in, in);
            __m128i out_high = _mm_unpackhi_epi32(in, in);
            _mm_storeu_si128((__m128i*) pOut, out_low);
            pOut += 4;
            _mm_storeu_si128((__m128i*) pOut, out_high);
            pOut += 4;
            pIn += 4;
        }

        while (pIn < pInEnd)
        {
            int32 in = *pIn++;
            *pOut++ = in;
            *pOut++ = in;
        }
    }

    void convertToStereo_sse(const float* inBuf, float* outBuf, int frames)
    {
        CK_ASSERT(inBuf != outBuf);
        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (frames & ~3);
        const float* pInEnd = inBuf + frames;
        float* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            __m128 in = _mm_loadu_ps(pIn);
            __m128 out_low = _mm_unpacklo_ps(in, in);
            __m128 out_high = _mm_unpackhi_ps(in, in);
            _mm_storeu_ps(pOut, out_low);
            pOut += 4;
            _mm_storeu_ps(pOut, out_high);
            pOut += 4;
            pIn += 4;
        }

        while (pIn < pInEnd)
        {
            float in = *pIn++;
            *pOut++ = in;
            *pOut++ = in;
        }
    }


    ////////////////////////////////////////
    // convert to stereo with scale

    void convertToStereoScale_sse(const int32* inBuf, int32* outBuf, int frames, float volume)
    {
        convertToStereoScale_default(inBuf, outBuf, frames, volume);
    }

    void convertToStereoScale_sse(const float* inBuf, float* outBuf, int frames, float volume)
    {
        __m128 v = _mm_set1_ps(volume);

        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (frames & ~3);
        const float* pInEnd = inBuf + frames;
        float* pOut = outBuf;

        while (pIn < pInEnd4)
        {
            __m128 in = _mm_loadu_ps(pIn);
            in = _mm_mul_ps(in, v);

            __m128 out_low = _mm_unpacklo_ps(in, in);
            __m128 out_high = _mm_unpackhi_ps(in, in);
            _mm_storeu_ps(pOut, out_low);
            pOut += 4;
            _mm_storeu_ps(pOut, out_high);
            pOut += 4;
            pIn += 4;
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

    void scale_sse(const int32* inBuf, int32* outBuf, int samples, float volume)
    {
        scale_default(inBuf, outBuf, samples, volume);
    }

    void scale_sse(const float* inBuf, float* outBuf, int samples, float volume)
    {
        __m128 v = _mm_set1_ps(volume);

        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + (samples & ~3);
        const float* pInEnd = inBuf + samples;
        float* pOut = outBuf;
        while (pIn < pInEnd4)
        {
            __m128 in = _mm_loadu_ps(pIn);
            in = _mm_mul_ps(in, v);
            _mm_storeu_ps(pOut, in);
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

    void stereoPan_sse(int32* buf, int frames, const VolumeMatrix& volume)
    {
        stereoPan_default(buf, frames, volume);
    }

    void stereoPan_sse(float* buf, int frames, const VolumeMatrix& volume)
    {
        float ll = volume.ll;
        float lr = volume.lr;
        float rl = volume.rl;
        float rr = volume.rr;

        float* p = buf;
        const float* pEnd8 = buf + ((frames * 2) & ~7);
        const float* pEnd = buf + frames * 2;

        // load constants
        __m128 llv = _mm_set1_ps(ll);
        __m128 lrv = _mm_set1_ps(lr);
        __m128 rlv = _mm_set1_ps(rl);
        __m128 rrv = _mm_set1_ps(rr);

        while (p < pEnd8)
        {
            // load 2 samples for each channel
            __m128 in0 = _mm_loadu_ps(p);
            __m128 in1 = _mm_loadu_ps(p+4);

            __m128 in_l = _mm_shuffle_ps(in0, in1, 0x88);
            __m128 in_r = _mm_shuffle_ps(in0, in1, 0xdd);

            __m128 tmp0;
            __m128 tmp1;

            tmp0 = _mm_mul_ps(in_l, llv);
            tmp1 = _mm_mul_ps(in_r, lrv);
            __m128 out_l = _mm_add_ps(tmp0, tmp1);
            tmp0 = _mm_mul_ps(in_l, rlv);
            tmp1 = _mm_mul_ps(in_r, rrv);
            __m128 out_r = _mm_add_ps(tmp0, tmp1);

            tmp0 = _mm_unpacklo_ps(out_l, out_r);
            tmp1 = _mm_unpackhi_ps(out_l, out_r);

            _mm_storeu_ps(p, tmp0);
            _mm_storeu_ps(p+4, tmp1);

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


    int stereoPanRamp_sse(int32* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        return stereoPanRamp_default(buf, frames, targetVolume, volume, volumeRate);
    }

    int stereoPanRamp_sse(float* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
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
        __m128 llv = _mm_setr_ps(ll, ll + llInc, ll + llInc*2, ll+llInc*3);
        __m128 lrv = _mm_setr_ps(lr, lr + lrInc, lr + lrInc*2, lr+lrInc*3);
        __m128 rlv = _mm_setr_ps(rl, rl + rlInc, rl + rlInc*2, rl+rlInc*3);
        __m128 rrv = _mm_setr_ps(rr, rr + rrInc, rr + rrInc*2, rr+rrInc*3);
        __m128 llIncv = _mm_set1_ps(llInc*4);
        __m128 lrIncv = _mm_set1_ps(lrInc*4);
        __m128 rlIncv = _mm_set1_ps(rlInc*4);
        __m128 rrIncv = _mm_set1_ps(rrInc*4);

        int nFrames = 0;
        while (p < pEnd8)
        {
            // load 4 samples for each channel, deinterleaving
            __m128 in0 = _mm_loadu_ps(p);
            __m128 in1 = _mm_loadu_ps(p+4);

            __m128 in_l = _mm_shuffle_ps(in0, in1, 0x88);
            __m128 in_r = _mm_shuffle_ps(in0, in1, 0xdd);

            __m128 tmp0;
            __m128 tmp1;

            tmp0 = _mm_mul_ps(in_l, llv);
            tmp1 = _mm_mul_ps(in_r, lrv);
            __m128 out_l = _mm_add_ps(tmp0, tmp1);
            tmp0 = _mm_mul_ps(in_l, rlv);
            tmp1 = _mm_mul_ps(in_r, rrv);
            __m128 out_r = _mm_add_ps(tmp0, tmp1);

            tmp0 = _mm_unpacklo_ps(out_l, out_r);
            tmp1 = _mm_unpackhi_ps(out_l, out_r);

            if (nFrames < llFrames) llv = _mm_add_ps(llv, llIncv);
            if (nFrames < lrFrames) lrv = _mm_add_ps(lrv, lrIncv);
            if (nFrames < rlFrames) rlv = _mm_add_ps(rlv, rlIncv);
            if (nFrames < rrFrames) rrv = _mm_add_ps(rrv, rrIncv);
            nFrames += 4;

            _mm_storeu_ps(p, tmp0);
            _mm_storeu_ps(p+4, tmp1);

            p += 8;
        }

        ll = Sse::getElement(llv, 0);
        lr = Sse::getElement(lrv, 0);
        rl = Sse::getElement(rlv, 0);
        rr = Sse::getElement(rrv, 0);

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

    void monoPan_sse(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& volume)
    {
        monoPan_default(inBuf, outBuf, frames, volume);
    }

    void monoPan_sse(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& volume)
    {
        float l = volume.ll;
        float r = volume.rr;

        const float* pIn = inBuf;
        const float* pInEnd4 = inBuf + ((frames) & ~3);
        const float* pInEnd = inBuf + frames;
        float* pOut = outBuf;

        // load constants
        __m128 lv = _mm_set1_ps(l);
        __m128 rv = _mm_set1_ps(r);

        while (pIn < pInEnd4)
        {
            __m128 in = _mm_loadu_ps(pIn);

            __m128 out_l = _mm_mul_ps(in, lv);
            __m128 out_r = _mm_mul_ps(in, rv);

            __m128 out0 = _mm_unpacklo_ps(out_l, out_r);
            __m128 out1 = _mm_unpackhi_ps(out_l, out_r);

            _mm_storeu_ps(pOut, out0);
            _mm_storeu_ps(pOut+4, out1);

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

    int monoPanRamp_sse(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        return monoPanRamp_default(inBuf, outBuf, frames, targetVolume, volume, volumeRate);
    }

    int monoPanRamp_sse(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
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
        __m128 lv = _mm_setr_ps(l, l + lInc, l + lInc*2, l+lInc*3);
        __m128 rv = _mm_setr_ps(r, r + rInc, r + rInc*2, r+rInc*3);
        __m128 lIncv = _mm_set1_ps(lInc*4);
        __m128 rIncv = _mm_set1_ps(rInc*4);

        int nFrames = 0;
        while (pIn < pInEnd4)
        {
            __m128 in = _mm_loadu_ps(pIn);

            __m128 out_l = _mm_mul_ps(in, lv);
            __m128 out_r = _mm_mul_ps(in, rv);

            __m128 out0 = _mm_unpacklo_ps(out_l, out_r);
            __m128 out1 = _mm_unpackhi_ps(out_l, out_r);

            if (nFrames < lFrames) lv = _mm_add_ps(lv, lIncv);
            if (nFrames < rFrames) rv = _mm_add_ps(rv, rIncv);
            nFrames += 4;

            _mm_storeu_ps(pOut, out0);
            pOut += 4;
            _mm_storeu_ps(pOut, out1);
            pOut += 4;

            pIn += 4;
        }

        l = Sse::getElement(lv, 0);
        r = Sse::getElement(rv, 0);

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

    void resampleMono_sse(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput)
    {
        resampleMono_default(inBuf, inFrames, outBuf, outFrames, lastInput);
    }

    void resampleMono_sse(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput)
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

        __m128 tov = { to, to+1, to+2, to+3 };
        __m128 dtov = _mm_set1_ps(4.0f);
        __m128 factorv = _mm_set1_ps(factor);
        __m128 fracv;
        __m128 sample0v = { 0 };
        __m128 sample1v = { 0 };
        __m128 tiv;
        __m128 ti0v;
        __m128i ti0vi;
        int* pti0vi = (int*) &ti0vi;
        __m128 out;

        while (pOut < pOutEnd4)
        {
            tiv = _mm_mul_ps(tov, factorv);
            ti0vi = _mm_cvttps_epi32(tiv);
            ti0v = _mm_cvtepi32_ps(ti0vi);
            fracv = _mm_sub_ps(tiv, ti0v);

            sample0v = _mm_set_ps(
                    inBuf[pti0vi[3]-1], 
                    inBuf[pti0vi[2]-1], 
                    inBuf[pti0vi[1]-1], 
                    inBuf[pti0vi[0]-1]);
            sample1v = _mm_set_ps(
                    inBuf[pti0vi[3]], 
                    inBuf[pti0vi[2]], 
                    inBuf[pti0vi[1]], 
                    inBuf[pti0vi[0]]);

            out = _mm_sub_ps(sample1v, sample0v);
            out = _mm_mul_ps(out, fracv);
            out = _mm_add_ps(sample0v, out);

            _mm_storeu_ps(pOut, out);
            pOut += 4;

            tov = _mm_add_ps(tov, dtov);
        }

        to = Sse::getElement(tov, 0);
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

    void resampleStereo_sse(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput)
    {
        resampleStereo_default(inBuf, inFrames, outBuf, outFrames, lastInput);
    }

    void resampleStereo_sse(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput)
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
        __m128 tov = { to, to+1, to+2, to+3 };
        __m128 dtov = _mm_set1_ps(4.0f);
        __m128 factorv = _mm_set1_ps(factor);
        __m128 fracv;
		__m128 sample0v_l;
		__m128 sample0v_r;
		__m128 sample1v_l;
		__m128 sample1v_r;
        __m128 tiv;
        __m128 ti0v;
        __m128i ti0vi;
        int* pti0vi = (int*) &ti0vi;

        while (pOut < pOutEnd8)
        {
            tiv = _mm_mul_ps(tov, factorv);
            ti0vi = _mm_cvttps_epi32(tiv);
            ti0v = _mm_cvtepi32_ps(ti0vi);
            fracv = _mm_sub_ps(tiv, ti0v);

            // load with _mm_loadu_ps, then swizzle
            sample0v_l = _mm_loadu_ps(inBuf + (pti0vi[0]-1)*2);
            sample0v_r = _mm_loadu_ps(inBuf + (pti0vi[1]-1)*2);
            sample1v_l = _mm_loadu_ps(inBuf + (pti0vi[2]-1)*2);
            sample1v_r = _mm_loadu_ps(inBuf + (pti0vi[3]-1)*2);
            _MM_TRANSPOSE4_PS(sample0v_l, sample0v_r, sample1v_l, sample1v_r);

            __m128 out_l;
            __m128 out_r;

            out_l = _mm_sub_ps(sample1v_l, sample0v_l);
            out_l = _mm_mul_ps(fracv, out_l);
            out_l = _mm_add_ps(sample0v_l, out_l);

            out_r = _mm_sub_ps(sample1v_r, sample0v_r);
            out_r = _mm_mul_ps(fracv, out_r);
            out_r = _mm_add_ps(sample0v_r, out_r);

            __m128 out0 = _mm_unpacklo_ps(out_l, out_r);
            __m128 out1 = _mm_unpackhi_ps(out_l, out_r);

            _mm_storeu_ps(pOut, out0);
            pOut += 4;
            _mm_storeu_ps(pOut, out1);
            pOut += 4;

            tov = _mm_add_ps(tov, dtov);
        }

        to = Sse::getElement(tov, 0);
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

    bool clamp_sse(const int32* inBuf, int32* outBuf, int samples)
    {
        return clamp_default(inBuf, outBuf, samples);
    }

    bool clamp_sse(const float* inBuf, float* outBuf, int samples)
    {
        __m128 minv = _mm_set1_ps(-1.0f);
        __m128 maxv = _mm_set1_ps(1.0f);
        __m128i clipFinal = _mm_setzero_si128();

        const float* pIn = inBuf;
        const float* pInEnd8 = inBuf + (samples & ~7);
        const float* pInEnd = inBuf + samples;
        float* pOut = outBuf;
        while (pIn < pInEnd8)
        {
            __m128 in = _mm_loadu_ps(pIn);

            __m128i clip;
            clip = CK_CAST_M128_M128I(_mm_cmpgt_ps(in, maxv));
            clipFinal = _mm_or_si128(clipFinal, clip);
            clip = CK_CAST_M128_M128I(_mm_cmplt_ps(in, minv));
            clipFinal = _mm_or_si128(clipFinal, clip);

            in = _mm_min_ps(in, maxv);
            in = _mm_max_ps(in, minv);

            _mm_storeu_ps(pOut, in);

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
            clip = clip || Sse::getElement(clipFinal, 0);
            clip = clip || Sse::getElement(clipFinal, 1);
            clip = clip || Sse::getElement(clipFinal, 2);
            clip = clip || Sse::getElement(clipFinal, 3);
        }

        return clip;
    }


}

}
#endif //  CK_X86_SSE


