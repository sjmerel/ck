#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

namespace Cki
{

struct VolumeMatrix;

namespace AudioUtil
{
    // TODO __restrict? versions of the in-place functions
    // TODO separate in-place versions?
    // TODO check for buffer overruuns

    // Convert format (in-place OK)
    // Note that the I32s are actually 8.24 fixed point
    void convert(const float* inBuf, int32* outBuf, int samples);
    void convertF32toI32_default(const float* inBuf, int32* outBuf, int samples);
    void convertF32toI32_neon(const float* inBuf, int32* outBuf, int samples);
    void convertF32toI32_sse(const float* inBuf, int32* outBuf, int samples);

    void convert(const float* inBuf, int16* outBuf, int samples);
    void convertF32toI16_default(const float* inBuf, int16* outBuf, int samples);
    void convertF32toI16_neon(const float* inBuf, int16* outBuf, int samples);
    void convertF32toI16_sse(const float* inBuf, int16* outBuf, int samples);

    void convert(const int32* inBuf, float* outBuf, int samples);
    void convertI32toF32_default(const int32* inBuf, float* outBuf, int samples);
    void convertI32toF32_neon(const int32* inBuf, float* outBuf, int samples);
    void convertI32toF32_sse(const int32* inBuf, float* outBuf, int samples);

    void convert(const int32* inBuf, int16* outBuf, int samples);
    void convertI32toI16_default(const int32* inBuf, int16* outBuf, int samples);
    void convertI32toI16_neon(const int32* inBuf, int16* outBuf, int samples);
    void convertI32toI16_sse(const int32* inBuf, int16* outBuf, int samples);

    void convert(const int16* inBuf, float* outBuf, int samples);
    void convertI16toF32_default(const int16* inBuf, float* outBuf, int samples);
    void convertI16toF32_neon(const int16* inBuf, float* outBuf, int samples);
    void convertI16toF32_sse(const int16* inBuf, float* outBuf, int samples);

    void convert(const int16* inBuf, int32* outBuf, int samples);
    void convertI16toI32_default(const int16* inBuf, int32* outBuf, int samples);
    void convertI16toI32_neon(const int16* inBuf, int32* outBuf, int samples);
    void convertI16toI32_sse(const int16* inBuf, int32* outBuf, int samples);

    void convert(const int8* inBuf, float* outBuf, int samples);
    void convertI8toF32_default(const int8* inBuf, float* outBuf, int samples);
    void convertI8toF32_neon(const int8* inBuf, float* outBuf, int samples);
    void convertI8toF32_sse(const int8* inBuf, float* outBuf, int samples);

    void convert(const int8* inBuf, int32* outBuf, int samples);
    void convertI8toI32_default(const int8* inBuf, int32* outBuf, int samples);
    void convertI8toI32_neon(const int8* inBuf, int32* outBuf, int samples);
    void convertI8toI32_sse(const int8* inBuf, int32* outBuf, int samples);

    void convert(const float* inBuf, float* outBuf, int samples);


    // Mix input into output (same number of channels)
    void mix(const int32* __restrict inBuf, int32* __restrict outBuf, int samples);
    void mix_default(const int32* __restrict inBuf, int32* __restrict outBuf, int samples);
    void mix_neon(const int32* __restrict inBuf, int32* __restrict outBuf, int samples);
    void mix_sse(const int32* __restrict inBuf, int32* __restrict outBuf, int samples);

    void mix(const float* __restrict inBuf, float* __restrict outBuf, int samples);
    void mix_default(const float* __restrict inBuf, float* __restrict outBuf, int samples);
    void mix_neon(const float* __restrict inBuf, float* __restrict outBuf, int samples);
    void mix_sse(const float* __restrict inBuf, float* __restrict outBuf, int samples);

    // Mix input into output (same number of channels), with volumes
    void mixVol(const int32* __restrict inBuf, float inVol, int32* __restrict outBuf, float outVol, int samples);
    void mixVol_default(const int32* __restrict inBuf, float inVol, int32* __restrict outBuf, float outVol, int samples);
    void mixVol_neon(const int32* __restrict inBuf, float inVol, int32* __restrict outBuf, float outVol, int samples);
    void mixVol_sse(const int32* __restrict inBuf, float inVol, int32* __restrict outBuf, float outVol, int samples);

    void mixVol(const float* __restrict inBuf, float inVol, float* __restrict outBuf, float outVol, int samples);
    void mixVol_default(const float* __restrict inBuf, float inVol, float* __restrict outBuf, float outVol, int samples);
    void mixVol_neon(const float* __restrict inBuf, float inVol, float* __restrict outBuf, float outVol, int samples);
    void mixVol_sse(const float* __restrict inBuf, float inVol, float* __restrict outBuf, float outVol, int samples);

    // Convert mono to stereo
    void convertToStereo(const int32* inBuf, int32* outBuf, int frames);
    void convertToStereo_default(const int32* inBuf, int32* outBuf, int frames);
    void convertToStereo_neon(const int32* inBuf, int32* outBuf, int frames);
    void convertToStereo_sse(const int32* inBuf, int32* outBuf, int frames);

    void convertToStereo(const float* inBuf, float* outBuf, int frames);
    void convertToStereo_default(const float* inBuf, float* outBuf, int frames);
    void convertToStereo_neon(const float* inBuf, float* outBuf, int frames);
    void convertToStereo_sse(const float* inBuf, float* outBuf, int frames);

    // Convert mono to stereo, scaling
    void convertToStereoScale(const int32* inBuf, int32* outBuf, int frames, float volume);
    void convertToStereoScale_default(const int32* inBuf, int32* outBuf, int frames, float volume);
    void convertToStereoScale_neon(const int32* inBuf, int32* outBuf, int frames, float volume);
    void convertToStereoScale_sse(const int32* inBuf, int32* outBuf, int frames, float volume);

    void convertToStereoScale(const float* inBuf, float* outBuf, int frames, float volume);
    void convertToStereoScale_default(const float* inBuf, float* outBuf, int frames, float volume);
    void convertToStereoScale_neon(const float* inBuf, float* outBuf, int frames, float volume);
    void convertToStereoScale_sse(const float* inBuf, float* outBuf, int frames, float volume);

    // Scale (in-place OK)
    void scale(const int32* inBuf, int32* outBuf, int samples, float volume);
    void scale_default(const int32* inBuf, int32* outBuf, int samples, float volume);
    void scale_neon(const int32* inBuf, int32* outBuf, int samples, float volume);
    void scale_sse(const int32* inBuf, int32* outBuf, int samples, float volume);

    void scale(const float* inBuf, float* outBuf, int samples, float volume);
    void scale_default(const float* inBuf, float* outBuf, int samples, float volume);
    void scale_neon(const float* inBuf, float* outBuf, int samples, float volume);
    void scale_sse(const float* inBuf, float* outBuf, int samples, float volume);

    // Apply pan matrix to stereo
    void stereoPan(int32* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    void stereoPan(int32* buf, int frames, const VolumeMatrix& volume);
    void stereoPan_default(int32* buf, int frames, const VolumeMatrix& volume);
    void stereoPan_neon(int32* buf, int frames, const VolumeMatrix& volume);
    void stereoPan_sse(int32* buf, int frames, const VolumeMatrix& volume);
    int stereoPanRamp(int32* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    int stereoPanRamp_default(int32* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    int stereoPanRamp_neon(int32* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    int stereoPanRamp_sse(int32* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);

    void stereoPan(float* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    void stereoPan(float* buf, int frames, const VolumeMatrix& volume);
    void stereoPan_default(float* buf, int frames, const VolumeMatrix& volume);
    void stereoPan_neon(float* buf, int frames, const VolumeMatrix& volume);
    void stereoPan_sse(float* buf, int frames, const VolumeMatrix& volume);
    int stereoPanRamp(float* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    int stereoPanRamp_default(float* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    int stereoPanRamp_neon(float* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    int stereoPanRamp_sse(float* buf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);

    // Apply pan matrix to mono, converting to stereo
    void monoPan(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    void monoPan(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& volume);
    void monoPan_default(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& volume);
    void monoPan_neon(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& volume);
    void monoPan_sse(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& volume);
    int monoPanRamp(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    int monoPanRamp_default(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    int monoPanRamp_neon(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    int monoPanRamp_sse(const int32* inBuf, int32* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);

    void monoPan(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    void monoPan(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& volume);
    void monoPan_default(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& volume);
    void monoPan_neon(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& volume);
    void monoPan_sse(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& volume);
    int monoPanRamp(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    int monoPanRamp_default(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    int monoPanRamp_neon(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);
    int monoPanRamp_sse(const float* inBuf, float* outBuf, int frames, const VolumeMatrix& targetVolume, VolumeMatrix& volume, float volumeRate);

    // Resample
    void resample(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int channels, int32* lastInput);
    void resampleMono(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput);
    void resampleMono_default(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput);
    void resampleMono_neon(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput);
    void resampleMono_sse(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput);
    void resampleStereo(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput);
    void resampleStereo_default(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput);
    void resampleStereo_neon(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput);
    void resampleStereo_sse(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int32* lastInput);

    void resample(const float* inBuf, int inFrames, float* outBuf, int outFrames, int channels, float* lastInput);
    void resampleMono(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput);
    void resampleMono_default(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput);
    void resampleMono_neon(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput);
    void resampleMono_sse(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput);
    void resampleStereo(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput);
    void resampleStereo_default(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput);
    void resampleStereo_neon(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput);
    void resampleStereo_sse(const float* inBuf, int inFrames, float* outBuf, int outFrames, float* lastInput);

    // Clamp 
    bool clamp(const int32* inBuf, int32* outBuf, int samples);
    bool clamp_default(const int32* inBuf, int32* outBuf, int samples);
    bool clamp_neon(const int32* inBuf, int32* outBuf, int samples);
    bool clamp_sse(const int32* inBuf, int32* outBuf, int samples);

    bool clamp(const float* inBuf, float* outBuf, int samples);
    bool clamp_default(const float* inBuf, float* outBuf, int samples);
    bool clamp_neon(const float* inBuf, float* outBuf, int samples);
    bool clamp_sse(const float* inBuf, float* outBuf, int samples);
}


}
