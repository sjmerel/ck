#pragma once

#include "ck/core/platform.h"
#include "ck/core/math.h"
#include "ck/audio/pcmi8decoder.h"
#include "ck/audio/pcmi16decoder.h"
#include "ck/audio/pcmf32decoder.h"
#include "ck/audio/adpcmdecoder.h"

namespace Cki
{

class AudioSource;

class DecoderBuf
{
public:
    DecoderBuf();
    ~DecoderBuf();

    void init(AudioSource&);

    Decoder* get() { return m_decoder; }
    const Decoder* get() const { return m_decoder; }

private:
    enum 
    { 
        k_memSize = Math::Max4<
             sizeof(PcmI8Decoder), 
             sizeof(PcmI16Decoder),
             sizeof(AdpcmDecoder), 
             sizeof(PcmF32Decoder)
                 >::value
    };
    Decoder* m_decoder;
    char m_mem[k_memSize];
};


}
