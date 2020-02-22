#pragma once

#include "ck/core/platform.h"
#include "ck/audio/decoder.h"

namespace Cki
{

class AudioSource;

class PcmI16Decoder : public Decoder
{
public:
    PcmI16Decoder(AudioSource&);
    virtual int decode(int32* buf, int frames);
    virtual int decode(float* buf, int frames);
    virtual bool isDone() const;
    virtual void setFramePos(int frame);
    virtual int getFramePos() const;
    virtual int getNumFrames() const;

private:
    template <typename T> int decodeImpl(T* buf, int frames);
};



}
