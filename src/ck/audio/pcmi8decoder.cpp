#include "ck/audio/pcmi8decoder.h"
#include "ck/audio/audioutil.h"
#include "ck/audio/audiosource.h"
#include "ck/core/debug.h"

namespace Cki
{


PcmI8Decoder::PcmI8Decoder(AudioSource& source) :
    Decoder(source)
{
}

int PcmI8Decoder::decode(int32* buf, int frames)
{
    return decodeImpl(buf, frames);
}

int PcmI8Decoder::decode(float* buf, int frames)
{
    return decodeImpl(buf, frames);
}

template <typename T> 
int PcmI8Decoder::decodeImpl(T* buf, int frames)
{
    if (isDone())
    {
        return 0;
    }
    else
    {
        int framesRead = m_source.read(buf, frames);

        int channels = m_source.getSampleInfo().channels;
        AudioUtil::convert((int8*)buf, buf, framesRead * channels);

        return framesRead;
    }
}

bool PcmI8Decoder::isDone() const
{
    return m_source.isDone();
}

void PcmI8Decoder::setFramePos(int frame)
{
    m_source.setBlockPos(frame); // since block == frame
}

int PcmI8Decoder::getFramePos() const
{
    return m_source.getBlockPos(); // since block == frame
}

int PcmI8Decoder::getNumFrames() const
{
    return m_source.getNumBlocks(); // since block == frame
}


}
