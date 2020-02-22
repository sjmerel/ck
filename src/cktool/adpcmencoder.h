#pragma once

#include "ck/core/platform.h"
#include "encoder.h"

namespace Cki
{


class AdpcmEncoder : public Encoder
{
public:
    AdpcmEncoder(int channels, int samplesPerBlock);
    virtual ~AdpcmEncoder();

    virtual int encode(const int16* inBuf, byte* outBuf, int frames, bool final); 
    virtual int getFramesPerBlock() const;
    virtual int getBytesPerBlock() const;

private:
    int m_samplesPerBlock;
    int16* m_input; // interleaved input data
    int m_inputFrames; 
    int m_delta[2]; // current initial delta for each channel
    byte* m_encoded[2]; // best encoding, and current encoding
    int16* m_decoded; // current decoded

    int encode(byte* outBuf);
    int findBestEncoding(byte* outBuf, int channel);

};


}
