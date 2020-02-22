#include "encoder.h"
#include "pcmi8encoder.h"
#include "pcmi16encoder.h"
#include "adpcmencoder.h"
#include "ck/core/debug.h"
#include "ck/audio/adpcm.h"

namespace Cki
{


Encoder::Encoder(int channels) :
    m_channels(channels)
{
}

Encoder* Encoder::create(AudioFormat::Value format, int channels)
{
    switch (format)
    {
        case AudioFormat::k_pcmI16:
            return new PcmI16Encoder(channels);

        case AudioFormat::k_pcmI8:
            return new PcmI8Encoder(channels);

        case AudioFormat::k_adpcm:
            return new AdpcmEncoder(channels, Adpcm::k_samplesPerBlock);

        default:
            CK_FAIL("unknown format");
            return NULL;
    }
}

}
