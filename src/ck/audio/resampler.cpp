#include "ck/audio/resampler.h"
#include "ck/audio/audioutil.h"

namespace Cki
{


Resampler::Resampler()
{
    resetState();
}

void Resampler::resample(const int32* inBuf, int inFrames, int32* outBuf, int outFrames, int channels)
{
    AudioUtil::resample(inBuf, inFrames, outBuf, outFrames, channels, (int32*) m_lastSample);
}

void Resampler::resample(const float* inBuf, int inFrames, float* outBuf, int outFrames, int channels)
{
    AudioUtil::resample(inBuf, inFrames, outBuf, outFrames, channels, (float*) m_lastSample);
}

void Resampler::resetState()
{
    m_lastSample[0] = 0;
    m_lastSample[1] = 0;
}

void Resampler::copyState(const Resampler& other)
{
    // XXX this isn't quite right, if sample rates are not the same
    m_lastSample[0] = other.m_lastSample[0];
    m_lastSample[1] = other.m_lastSample[1];
}


}
