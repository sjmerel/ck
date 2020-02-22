#include "ck/audio/pcmf32decoder.h"
#include "ck/audio/audioutil.h"
#include "ck/audio/audiosource.h"
#include "ck/core/debug.h"

namespace Cki
{


PcmF32Decoder::PcmF32Decoder(AudioSource& source) :
    Decoder(source)
{
}

int PcmF32Decoder::decode(int32* buf, int frames)
{
    return decodeImpl(buf, frames);
}

int PcmF32Decoder::decode(float* buf, int frames)
{
    return decodeImpl(buf, frames);
}

template <typename T> 
int PcmF32Decoder::decodeImpl(T* buf, int frames)
{
    if (isDone())
    {
        return 0;
    }
    else
    {
        int framesRead = m_source.read(buf, frames);

        int channels = m_source.getSampleInfo().channels;
        AudioUtil::convert((float*)buf, buf, framesRead * channels);

        return framesRead;
    }
}

bool PcmF32Decoder::isDone() const
{
    return m_source.isDone();
}

void PcmF32Decoder::setFramePos(int frame)
{
    m_source.setBlockPos(frame); // since block == frame
}

int PcmF32Decoder::getFramePos() const
{
    return m_source.getBlockPos(); // since block == frame
}

int PcmF32Decoder::getNumFrames() const
{
    return m_source.getNumBlocks(); // since block == frame
}


}

