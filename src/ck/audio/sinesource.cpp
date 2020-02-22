#include "ck/audio/sinesource.h"
#include "ck/core/math.h"

namespace Cki
{


SineSource::SineSource(int channels, int sampleRateHz, float freqHz) :
    m_channels(channels),
    m_sampleRate(sampleRateHz),
    m_pos(0)
{
    // XXX make period a whole number of samples
    m_period = Math::round(m_sampleRate / freqHz);
}

int SineSource::read(int16* buf, int frames)
{
    for (int frame = 0; frame < frames; ++frame)
    {
        float value = Math::sin(m_pos * Math::k_2pi / m_period);
        int16 sample = (int16) (value * CK_INT16_MAX);
        for (int chan = 0; chan < m_channels; ++chan)
        {
            *buf++ = sample;
        }
        ++m_pos;
    }
    if (m_pos >= m_period)
    {
        m_pos = m_pos % m_period;
    }
    return frames;
}

void SineSource::reset()
{
    m_pos = 0;
}

int SineSource::getChannels() const
{
    return m_channels;
}

int SineSource::getSampleRate() const
{
    return m_sampleRate;
}

bool SineSource::isReady() const
{
    return true;
}

bool SineSource::isDone() const
{
    return false;
}


}
