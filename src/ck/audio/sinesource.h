#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/audio/audiosource.h"

namespace Cki
{


class SineSource : public AudioSource
{
public:
    SineSource(int channels, int sampleRateHz, float freqHz);

    virtual int read(int16* buf, int frames);
    virtual void reset();
    virtual int getChannels() const;
    virtual int getSampleRate() const;
    virtual bool isReady() const;
    virtual bool isDone() const;

private:
    int m_channels;
    int m_sampleRate;
    int m_period;
    int m_pos;
};


}

