#include "ck/audio/audioutil.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/mem.h"
#include "ck/core/fixedpoint.h"
#include "ck/core/system.h"
#include "ck/audio/volumematrix.h"

#if CK_PLATFORM_ANDROID
#  include "ck/core/system_android.h"
#endif

// On IOS, all armv7 devices support NEON, so if compiled with NEON, use it.
// On Android, we have to do a check at runtime.
// On WP8, assuming all devices support NEON.
//
// NEON implementations are in audioutil_neon.cpp, since they must be compiled
// separately with -mfpu=neon for Android.
// 
// Assembly implementations were much better than intrinsics on Android when compiling
// with GCC; now that we're using clang, the differences are much smaller, but still we
// might as well choose the faster one.  For WP8, inline assembly is not currently supported
// by Visual Studio 2012, so we're just using intrinsics.
//
// TODO: figure out why some assembly implementations are so much faster by looking at 
// disassembly of non-asm version, and see if non-asm version can be improved

#if CK_ARCH_ARM
#  if CK_PLATFORM_ANDROID && __ARM_ARCH_5TE__
#    define SIMD_IMPL(func, ...) func ## _default(__VA_ARGS__)
#  else
#    define SIMD_IMPL(func, ...) System::get()->isSimdSupported() ? func ## _neon(__VA_ARGS__) : func ## _default(__VA_ARGS__)  // ternary, so "return SIMD_IMPL(...)" is ok
#  endif
#elif CK_ARCH_X86
#  define SIMD_IMPL(func, ...) System::get()->isSimdSupported() ? func ## _sse(__VA_ARGS__) : func ## _default(__VA_ARGS__)
#else 
#  define SIMD_IMPL(func, ...) func ## _default(__VA_ARGS__)
#endif


namespace Cki
{


namespace AudioUtil
{

    ////////////////////////////////////////
    // convert float to int32

    void convert(const float* inBuf, int32* outBuf, int samples)
    {
        SIMD_IMPL(convertF32toI32, inBuf, outBuf, samples);
    }

    void convertF32toI32_default(const float* inBuf, int32* outBuf, int samples)
    {
        const float* pIn = inBuf;
        const float* pInEnd = inBuf + samples;
        int32* pOut = outBuf;
        while (pIn < pInEnd)
        {
            *pOut++ = (int32) (*pIn++ * (1 << 24));
        }
    }


    ////////////////////////////////////////
    // convert float to int16

    void convert(const float* inBuf, int16* outBuf, int samples)
    {
        SIMD_IMPL(convertF32toI16, inBuf, outBuf, samples);
    }

    void convertF32toI16_default(const float* inBuf, int16* outBuf, int samples)
    {
        const float* pIn = inBuf;
        const float* pInEnd = inBuf + samples;
        int16* pOut = outBuf;
        while (pIn < pInEnd)
        {
            *pOut++ = (int16) (*pIn++ * CK_INT16_MAX);
        }
    }


    ////////////////////////////////////////
    // convert int32 to float

    void convert(const int32* inBuf, float* outBuf, int samples)
    {
        SIMD_IMPL(convertI32toF32, inBuf, outBuf, samples);
    }

    void convertI32toF32_default(const int32* inBuf, float* outBuf, int samples)
    {
        const int32* pIn = inBuf;
        const int32* pInEnd = inBuf + samples;
        float* pOut = outBuf;
        float factor = 1.0f / (1 << 24);
        while (pIn < pInEnd)
        {
            *pOut++ = (float) *pIn++ * factor;
        }
    }


    ////////////////////////////////////////
    // convert int32 to int16

    void convert(const int32* inBuf, int16* outBuf, int samples)
    {
        SIMD_IMPL(convertI32toI16, inBuf, outBuf, samples);
    }

    void convertI32toI16_default(const int32* inBuf, int16* outBuf, int samples)
    {
        const int32* pIn = inBuf;
        const int32* pInEnd = inBuf + samples;
        int16* pOut = outBuf;
        while (pIn < pInEnd)
        {
            *pOut++ = (int16) (*pIn++ >> 9); // 9 because of sign bit
        }
    }


    ////////////////////////////////////////
    // convert int16 to float

    void convert(const int16* inBuf, float* outBuf, int samples)
    {
        SIMD_IMPL(convertI16toF32, inBuf, outBuf, samples);
    }

    void convertI16toF32_default(const int16* inBuf, float* outBuf, int samples)
    {
        // iterate backwards so we can do it in-place
        const int16* pIn = inBuf + samples - 1;
        float* pOut = outBuf + samples - 1;
        float factor = 1.0f / CK_INT16_MAX;
        while (pIn >= inBuf)
        {
            *pOut-- = ((float) *pIn--) * factor;
        }
    }


    ////////////////////////////////////////
    // convert int16 to int32

    void convert(const int16* inBuf, int32* outBuf, int samples)
    {
        SIMD_IMPL(convertI16toI32, inBuf, outBuf, samples);
    }

    void convertI16toI32_default(const int16* inBuf, int32* outBuf, int samples)
    {
        // iterate backwards so we can do it in-place
        const int16* pIn = inBuf + samples - 1;
        int32* pOut = outBuf + samples - 1;
        while (pIn >= inBuf)
        {
            *pOut-- = ((int32) *pIn--) << 9; // 9 because of sign bit
        }
    }


    ////////////////////////////////////////
    // convert int8 to float

    void convert(const int8* inBuf, float* outBuf, int samples)
    {
        SIMD_IMPL(convertI8toF32, inBuf, outBuf, samples); 
    }

    void convertI8toF32_default(const int8* inBuf, float* outBuf, int samples)
    {
        // iterate backwards so we can do it in-place
        const int8* pIn = inBuf + samples - 1;
        float* pOut = outBuf + samples - 1;
        float factor = 1.0f / CK_INT8_MAX;
        while (pIn >= inBuf)
        {
            *pOut-- = ((float) *pIn--) * factor;
        }
    }


    ////////////////////////////////////////
    // convert int8 to int32

    void convert(const int8* inBuf, int32* outBuf, int samples)
    {
        SIMD_IMPL(convertI8toI32, inBuf, outBuf, samples); 
    }

    void convertI8toI32_default(const int8* inBuf, int32* outBuf, int samples)
    {
        // iterate backwards so we can do it in-place
        const int8* pIn = inBuf + samples - 1;
        int32* pOut = outBuf + samples - 1;
        while (pIn >= inBuf)
        {
            *pOut-- = ((int32) *pIn--) << 17;
        }
    }


    ////////////////////////////////////////
    // dummy float-float conversion

    void convert(const float* inBuf, float* outBuf, int samples)
    {
        Mem::copy(outBuf, inBuf, samples * sizeof(float));
    }


    ////////////////////////////////////////
    // mix

    void mix(const int32* __restrict inBuf, int32* __restrict outBuf, int samples)
    {
        SIMD_IMPL(mix, inBuf, outBuf, samples);
    }

    void mix_default(const int32* __restrict inBuf, int32* __restrict outBuf, int samples)
    {
        CK_ASSERT(inBuf != outBuf);
        const int32* pIn = inBuf;
        const int32* pInEnd = inBuf + samples;
        int32* pOut = outBuf;
        while (pIn < pInEnd)
        {
            *pOut++ += *pIn++;
        }
    }

    void mix(const float* __restrict inBuf, float* __restrict outBuf, int samples)
    {
        SIMD_IMPL(mix, inBuf, outBuf, samples);
    }

    void mix_default(const float* __restrict inBuf, float* __restrict outBuf, int samples)
    {
        CK_ASSERT(inBuf != outBuf);
        const float* pIn = inBuf;
        const float* pInEnd = inBuf + samples;
        float* pOut = outBuf;
        while (pIn < pInEnd)
        {
            *pOut++ += *pIn++;
        }
    }


    ////////////////////////////////////////
    // mix with volumes

    void mixVol(const int32* __restrict inBuf, float inVol, int32* __restrict outBuf, float outVol, int samples)
    {
        SIMD_IMPL(mixVol, inBuf, inVol, outBuf, outVol, samples);
    }

    void mixVol_default(const int32* __restrict inBuf, float inVol, int32* __restrict outBuf, float outVol, int samples)
    {
        CK_ASSERT(inBuf != outBuf);
        int64 inVol_8_24 = Fixed8_24::fromFloat(inVol);
        int64 outVol_8_24 = Fixed8_24::fromFloat(outVol);
        const int32* pIn = inBuf;
        const int32* pInEnd = inBuf + samples;
        int32* pOut = outBuf;
        while (pIn < pInEnd)
        {
            int64 in = *pIn;
            int64 out = *pOut;
            *pOut = (int32) ((in * inVol_8_24 + out * outVol_8_24) >> 24);
            ++pIn;
            ++pOut;
        }
    }

    void mixVol(const float* __restrict inBuf, float inVol, float* __restrict outBuf, float outVol, int samples)
    {
        SIMD_IMPL(mixVol, inBuf, inVol, outBuf, outVol, samples);
    }

    void mixVol_default(const float* __restrict inBuf, float inVol, float* __restrict outBuf, float outVol, int samples)
    {
        CK_ASSERT(inBuf != outBuf);
        const float* pIn = inBuf;
        const float* pInEnd = inBuf + samples;
        float* pOut = outBuf;
        while (pIn < pInEnd)
        {
            *pOut = *pIn * inVol + *pOut * outVol;
            ++pIn;
            ++pOut;
        }
    }


    ////////////////////////////////////////
    // convert to stereo

    void convertToStereo(const int32* inBuf, int32* outBuf, int frames)
    {
        SIMD_IMPL(convertToStereo, inBuf, outBuf, frames);
    }

    void convertToStereo_default(const int32* inBuf, int32* outBuf, int frames)
    {
        CK_ASSERT(inBuf != outBuf);
        const int32* pIn = inBuf;
        const int32* pInEnd = inBuf + frames;
        int32* pOut = outBuf;
        while (pIn < pInEnd)
        {
            int32 in = *pIn++;
            *pOut++ = in;
            *pOut++ = in;
        }
    }

    void convertToStereo(const float* inBuf, float* outBuf, int frames)
    {
        SIMD_IMPL(convertToStereo, inBuf, outBuf, frames);
    }

    void convertToStereo_default(const float* inBuf, float* outBuf, int frames)
    {
        CK_ASSERT(inBuf != outBuf);
        const float* pIn = inBuf;
        const float* pInEnd = inBuf + frames;
        float* pOut = outBuf;
        while (pIn < pInEnd)
        {
            float in = *pIn++;
            *pOut++ = in;
            *pOut++ = in;
        }
    }


    ////////////////////////////////////////
    // convert to stereo with scale

    void convertToStereoScale(const int32* inBuf, int32* outBuf, int frames, float volume)
    {
        SIMD_IMPL(convertToStereoScale, inBuf, outBuf, frames, volume);
    }

    void convertToStereoScale_default(const int32* inBuf, int32* outBuf, int frames, float volume)
    {
        int64 v = Fixed8_24::fromFloat(volume);

        const int32* pIn = inBuf;
        const int32* pInEnd = inBuf + frames;
        int32* pOut = outBuf;
        while (pIn < pInEnd)
        {
            int32 out = (int32) ((*pIn++ * v) >> 24);
            *pOut++ = out;
            *pOut++ = out;
        }
    }

    void convertToStereoScale(const float* inBuf, float* outBuf, int frames, float volume)
    {
        SIMD_IMPL(convertToStereoScale, inBuf, outBuf, frames, volume);
    }

    void convertToStereoScale_default(const float* inBuf, float* outBuf, int frames, float volume)
    {
        const float* pIn = inBuf;
        const float* pInEnd = inBuf + frames;
        float* pOut = outBuf;
        while (pIn < pInEnd)
        {
            float out = *pIn++ * volume;
            *pOut++ = out;
            *pOut++ = out;
        }
    }


    ////////////////////////////////////////
    // scale

    void scale(const int32* inBuf, int32* outBuf, int samples, float volume)
    {
        SIMD_IMPL(scale, inBuf, outBuf, samples, volume);
    }

    void scale_default(const int32* inBuf, int32* outBuf, int samples, float volume)
    {
        int64 voli = Fixed8_24::fromFloat(volume);

        const int32* pIn = inBuf;
        const int32* pInEnd = inBuf + samples;
        int32* pOut = outBuf;
        while (pIn < pInEnd)
        {
            *pOut++ = (int32) ((*pIn++ * voli) >> 24);
        }
    }

    void scale(const float* inBuf, float* outBuf, int samples, float volume)
    {
        SIMD_IMPL(scale, inBuf, outBuf, samples, volume);
    }

    void scale_default(const float* inBuf, float* outBuf, int samples, float volume)
    {
        const float* pIn = inBuf;
        const float* pInEnd = inBuf + samples;
        float* pOut = outBuf;
        while (pIn < pInEnd)
        {
            *pOut++ = *pIn++ * volume;
        }
    }


    ////////////////////////////////////////
    // stereo pan

    // TODO: optimizations:
    //  - special versions for typical stereo pan values
    //  - ramping versions of all the special case optimizations

    template <typename T>
    static void stereoPanImpl(T* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        if (targetVolume.equals(volume, 0.001f))
        {
            // volume is not ramping
            volume = targetVolume;

            if (volume.isScalar())
            {
                // scalar volume (i.e. stereo pan=0)
                float s = (volume.ll + volume.rr) * 0.5f;
                if (!Math::floatEqual(s, 1.0f, 0.001f))
                {
                    scale(buf, buf, frames*2, s);
                }
            }
            else
            {
                stereoPan(buf, frames, volume);
            }
        }
        else
        {
            // ramp the volumes
            int rampFrames = stereoPanRamp(buf, frames, targetVolume, volume, volumeRate);
            int framesLeft = frames - rampFrames;
            if (framesLeft > 0)
            {
                stereoPan(buf + rampFrames * 2, framesLeft, volume);
            }
        }
    }

    void stereoPan(int32* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        stereoPanImpl<int32>(buf, frames, targetVolume, volume, volumeRate);
    }

    void stereoPan(int32* buf, int frames, const VolumeMatrix& volume)
    {
        SIMD_IMPL(stereoPan, buf, frames, volume);
    }

    void stereoPan_default(int32* buf, int frames, const VolumeMatrix& volume)
    {
        int32 ll = Fixed8_24::fromFloat(volume.ll);
        int32 lr = Fixed8_24::fromFloat(volume.lr);
        int32 rl = Fixed8_24::fromFloat(volume.rl);
        int32 rr = Fixed8_24::fromFloat(volume.rr);

        int32* p = buf;
        int32* pEnd = buf + frames * 2;
        while (p < pEnd)
        {
            int64 in_l = *p;
            int64 in_r = *(p+1);

            *p++ = (int32) ((in_l * ll + in_r * lr) >> 24);
            *p++ = (int32) ((in_l * rl + in_r * rr) >> 24);
        }
    }

    int stereoPanRamp(int32* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        return SIMD_IMPL(stereoPanRamp, buf, frames, targetVolume, volume, volumeRate);
    }

    int stereoPanRamp_default(int32* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        VolumeMatrix volDiff = targetVolume - volume;

        int llFrames = Math::abs((int) (volDiff.ll / volumeRate));
        int lrFrames = Math::abs((int) (volDiff.lr / volumeRate));
        int rlFrames = Math::abs((int) (volDiff.rl / volumeRate));
        int rrFrames = Math::abs((int) (volDiff.rr / volumeRate));

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

        int nFrames = 0;
        int32* p = buf;
        int32* pEnd = buf + rampFrames * 2;
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

    void stereoPan(float* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        stereoPanImpl<float>(buf, frames, targetVolume, volume, volumeRate);
    }

    void stereoPan(float* buf, int frames, const VolumeMatrix& volume)
    {
        SIMD_IMPL(stereoPan, buf, frames, volume);
    }

    void stereoPan_default(float* buf, int frames, const VolumeMatrix& volume)
    {
        float ll = volume.ll;
        float lr = volume.lr;
        float rl = volume.rl;
        float rr = volume.rr;

        float* p = buf;
        float* pEnd = buf + frames * 2;
        while (p < pEnd)
        {
            float in_l = *p;
            float in_r = *(p+1);

            *p++ = in_l * ll + in_r * lr;
            *p++ = in_l * rl + in_r * rr;
        }
    }

    int stereoPanRamp(float* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        return SIMD_IMPL(stereoPanRamp, buf, frames, targetVolume, volume, volumeRate);
    }

    int stereoPanRamp_default(float* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        VolumeMatrix volDiff = targetVolume - volume;

        int llFrames = Math::abs((int) (volDiff.ll / volumeRate));
        int lrFrames = Math::abs((int) (volDiff.lr / volumeRate));
        int rlFrames = Math::abs((int) (volDiff.rl / volumeRate));
        int rrFrames = Math::abs((int) (volDiff.rr / volumeRate));

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

        int nFrames = 0;
        float* p = buf;
        const float* pEnd = buf + rampFrames * 2;
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
        volume.ll = nFrames >= llFrames ? targetVolume.ll : ll;
        volume.lr = nFrames >= lrFrames ? targetVolume.lr : lr;
        volume.rl = nFrames >= rlFrames ? targetVolume.rl : rl;
        volume.rr = nFrames >= rrFrames ? targetVolume.rr : rr;

        return rampFrames;
    }


    ////////////////////////////////////////
    // mono pan

    template <typename T>
    static void monoPanImpl(const T* inBuf, T* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        if (Math::floatEqual(volume.ll, targetVolume.ll, 0.001f) && Math::floatEqual(volume.rr, targetVolume.rr, 0.001f))
        {
            // volume is not ramping
            volume = targetVolume;

            if (Math::floatEqual(volume.ll, volume.rr, 0.001f))
            {
                // scalar volume
                if (Math::floatEqual(volume.ll, 1.0f, 0.001f))
                {
                    convertToStereo(inBuf, outBuf, frames);
                }
                else
                {
                    convertToStereoScale(inBuf, outBuf, frames, (volume.ll+volume.rr)*0.5f);
                }
            }
            else
            {
                monoPan(inBuf, outBuf, frames, volume);
            }
        }
        else
        {
            // ramp the volumes
            int rampFrames = monoPanRamp(inBuf, outBuf, frames, targetVolume, volume, volumeRate);
            int framesLeft = frames - rampFrames;
            if (framesLeft > 0)
            {
                monoPan(inBuf + rampFrames, outBuf + rampFrames * 2, framesLeft, volume);
            }
        }
    }

    void monoPan(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        monoPanImpl<int32>(inBuf, outBuf, frames, targetVolume, volume, volumeRate);
    }

    void monoPan(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& volume)
    {
        SIMD_IMPL(monoPan, inBuf, outBuf, frames, volume);
    }

    void monoPan_default(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& volume)
    {
        int32 l = Fixed8_24::fromFloat(volume.ll);
        int32 r = Fixed8_24::fromFloat(volume.rr);

        const int32* pIn = inBuf;
        const int32* pInEnd = inBuf + frames;
        int32* pOut = outBuf;
        while (pIn < pInEnd)
        {
            int64 in = *pIn++;

            *pOut++ = (int32) ((in * l) >> 24);
            *pOut++ = (int32) ((in * r) >> 24);
        }
    }

    int monoPanRamp(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        return SIMD_IMPL(monoPanRamp, inBuf, outBuf, frames, targetVolume, volume, volumeRate);
    }

    int monoPanRamp_default(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        float lDiff = targetVolume.ll - volume.ll;
        float rDiff = targetVolume.rr - volume.rr;

        int lFrames = Math::abs((int) (lDiff / volumeRate));
        int rFrames = Math::abs((int) (rDiff / volumeRate));

        int rampFrames = Math::max(lFrames, rFrames);
        rampFrames = Math::min(rampFrames, frames);

        int32 lInc = lFrames > 0 ? Fixed8_24::fromFloat(lDiff / lFrames) : 0;
        int32 rInc = rFrames > 0 ? Fixed8_24::fromFloat(rDiff / rFrames) : 0;

        int32 l = Fixed8_24::fromFloat(volume.ll);
        int32 r = Fixed8_24::fromFloat(volume.rr);

        int nFrames = 0;
        const int32* pIn = inBuf;
        const int32* pInEnd = inBuf + rampFrames;
        int32* pOut = outBuf;
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

    void monoPan(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        monoPanImpl<float>(inBuf, outBuf, frames, targetVolume, volume, volumeRate);
    }

    void monoPan(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& volume)
    {
        SIMD_IMPL(monoPan, inBuf, outBuf, frames, volume);
    }

    void monoPan_default(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& volume)
    {
        float l = volume.ll;
        float r = volume.rr;

        const float* pIn = inBuf;
        const float* pInEnd = inBuf + frames;
        float* pOut = outBuf;
        while (pIn < pInEnd)
        {
            float in = *pIn++;
            *pOut++ = in * l;
            *pOut++ = in * r;
        }
    }

    int monoPanRamp(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        return SIMD_IMPL(monoPanRamp, inBuf, outBuf, frames, targetVolume, volume, volumeRate);
    }

    int monoPanRamp_default(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate)
    {
        float lDiff = targetVolume.ll - volume.ll;
        float rDiff = targetVolume.rr - volume.rr;

        int lFrames = Math::abs((int) (lDiff / volumeRate));
        int rFrames = Math::abs((int) (rDiff / volumeRate));

        int rampFrames = Math::max(lFrames, rFrames);
        rampFrames = Math::min(rampFrames, frames);

        float lInc = lFrames > 0 ? lDiff / lFrames : 0;
        float rInc = rFrames > 0 ? rDiff / rFrames : 0;

        float l = volume.ll;
        float r = volume.rr;

        int nFrames = 0;
        const float* pIn = inBuf;
        const float* pInEnd = inBuf + rampFrames;
        float* pOut = outBuf;
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

    template <typename T>
    static void resampleImpl(const T* inBuf, int inFrames, T* outBuf, int outFrames, int channels, T* lastInput)
    {
        if (channels == 1)
        {
            resampleMono(inBuf, inFrames, outBuf, outFrames, lastInput);
        }
        else
        {
            resampleStereo(inBuf, inFrames, outBuf, outFrames, lastInput);
        }
    }

    void resample(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int channels, int32* lastInput)
    {
        resampleImpl<int32>(inBuf, inFrames, outBuf, outFrames, channels, lastInput);
    }

    void resampleMono(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput)
    {
        SIMD_IMPL(resampleMono, inBuf, inFrames, outBuf, outFrames, lastInput);
    }

    void resampleMono_default(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput)
    {
        int32* pOut = outBuf;
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
        const int32* pIn = inBuf + (t0-1);
        int prevt0 = t0;
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

    void resampleStereo(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput)
    {
        SIMD_IMPL(resampleStereo, inBuf, inFrames, outBuf, outFrames, lastInput);
    }

    void resampleStereo_default(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput)
    {
        int32* pOut = outBuf;
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
        const int32* pIn = inBuf + 2*(t0-1);
        int prevt0 = t0;
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

    void resample(const float* inBuf, int inFrames, float* outBuf, int outFrames, int channels, float* lastInput)
    {
        resampleImpl<float>(inBuf, inFrames, outBuf, outFrames, channels, lastInput);
    }

    void resampleMono(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput)
    {
        SIMD_IMPL(resampleMono, inBuf, inFrames, outBuf, outFrames, lastInput);
    }

    void resampleMono_default(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput)
    {
        float* pOut = outBuf;
        const float* pOutEnd = outBuf + outFrames;

        float factor = (float) inFrames / outFrames;

        float ti = 0.0f; // input frame
        int ti0 = 0;     // whole number part of input frame
        float to = 0.0f; // output frame
        float frac = 0.0f;

        // interpolate between last sample of previous slice and first sample of current slice
        float sample0 = lastInput[0];
        float sample1 = inBuf[0];
        while (ti0 == 0)
        {
            *pOut++ = sample0 + frac * (sample1 - sample0);

            ti = ++to * factor;
            ti0 = (int) ti;
            frac = ti - ti0;
        }

        // resample the rest of the current slice
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

    void resampleStereo(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput)
    {
        SIMD_IMPL(resampleStereo, inBuf, inFrames, outBuf, outFrames, lastInput);
    }

    void resampleStereo_default(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput)
    {
        float* pOut = outBuf;
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

    bool clamp(const int32* inBuf, int32* outBuf, int samples)
    {
        return SIMD_IMPL(clamp, inBuf, outBuf, samples);
    }

    bool clamp_default(const int32* inBuf, int32* outBuf, int samples)
    {
        const int32 k_min = ((uint32)-1 <<24) + 1;
        const int32 k_max = (1<<24) - 1;
        bool clip = false;

        const int32* pIn = inBuf;
        const int32* pInEnd = pIn + samples;
        int32* pOut = outBuf;
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

        return clip;
    }

    bool clamp(const float* inBuf, float* outBuf, int samples)
    {
        return SIMD_IMPL(clamp, inBuf, outBuf, samples);
    }

    bool clamp_default(const float* inBuf, float* outBuf, int samples)
    {
        bool clip = false;

        const float* pIn = inBuf;
        const float* pInEnd = pIn + samples;
        float* pOut = outBuf;
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

        return clip;
    }

}

}
