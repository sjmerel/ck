#pragma once

#include "ck/core/platform.h"
#include "ck/audio/decoder.h"
#include "ck/audio/adpcm.h"

namespace Cki
{


class AdpcmDecoder : public Decoder
{
public:
    AdpcmDecoder(AudioSource&);
    virtual int decode(int32* buf, int frames);
    virtual int decode(float* buf, int frames);
    virtual bool isDone() const;
    virtual void setFramePos(int frame);
    virtual int getFramePos() const;
    virtual int getNumFrames() const;

private:
    int16 m_decoded[Adpcm::k_samplesPerBlock*2];
    int m_decodedBlock; // which block m_decoded is from
    int m_frameOffset; // offset of next frame in m_decoded
    bool m_valid; // true if decoded data is valid

    bool decodeBlock();
    template <typename T> int decodeImpl(T* buf, int frames);
};


}
