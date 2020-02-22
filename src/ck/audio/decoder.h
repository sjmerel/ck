#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

namespace Cki
{

class AudioSource;

class Decoder
{
public:
    Decoder(AudioSource&);
    virtual ~Decoder() {}

    virtual int decode(int32* buf, int frames) = 0;
    virtual int decode(float* buf, int frames) = 0;
    virtual bool isDone() const = 0;

    virtual void setFramePos(int frame) = 0;
    virtual int getFramePos() const = 0;
    virtual int getNumFrames() const = 0;

protected:
    AudioSource& m_source;
};


}
