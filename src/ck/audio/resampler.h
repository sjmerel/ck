#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/audio/varsample.h"

namespace Cki
{


class Resampler
{
public:
    Resampler();

    void resample(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int channels);
    void resample(const float* inBuf, int inFrames, float* outBuf, int outFrames, int channels);

    void resetState();
    void copyState(const Resampler&);

private:
    VarSample m_lastSample[2];
};


}
