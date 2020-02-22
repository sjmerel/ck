#include "ck/audio/pcmi16decoder.h"
#include "ck/audio/audioutil.h"
#include "ck/audio/audiosource.h"

namespace Cki
{


PcmI16Decoder::PcmI16Decoder(AudioSource& source) :
    Decoder(source)
{
}

int PcmI16Decoder::decode(int32* buf, int frames)
{
    return decodeImpl(buf, frames);
}

int PcmI16Decoder::decode(float* buf, int frames)
{
    return decodeImpl(buf, frames);
}

template <typename T> 
int PcmI16Decoder::decodeImpl(T* buf, int frames)
{
    if (isDone())
    {
        return 0;
    }
    else
    {
        int framesRead = m_source.read(buf, frames);

        int channels = m_source.getSampleInfo().channels;
        AudioUtil::convert((int16*) buf, buf, framesRead * channels);

        return framesRead;
    }
}

bool PcmI16Decoder::isDone() const
{
    return m_source.isDone();
}

void PcmI16Decoder::setFramePos(int frame)
{
    m_source.setBlockPos(frame); // since block == frame
}

int PcmI16Decoder::getFramePos() const
{
    return m_source.getBlockPos(); // since block == frame
}

int PcmI16Decoder::getNumFrames() const
{
    return m_source.getNumBlocks(); // since block == frame
}



}
