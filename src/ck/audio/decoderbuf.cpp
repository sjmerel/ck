#include "ck/audio/decoderbuf.h"
#include "ck/audio/audiosource.h"
#include "ck/audio/audioformat.h"
#include "ck/core/debug.h"
#include <new>

namespace Cki
{


DecoderBuf::DecoderBuf() :
    m_decoder(NULL)
{}

DecoderBuf::~DecoderBuf()
{
    if (m_decoder)
    {
        m_decoder->~Decoder();
    }
}

void DecoderBuf::init(AudioSource& source)
{
    CK_ASSERT(!m_decoder);
    switch (source.getSampleInfo().format)
    {
        case AudioFormat::k_pcmI16:
            m_decoder = new (m_mem) PcmI16Decoder(source);
            break;

        case AudioFormat::k_pcmI8:
            m_decoder = new (m_mem) PcmI8Decoder(source);
            break;

        case AudioFormat::k_pcmF32:
            m_decoder = new (m_mem) PcmF32Decoder(source);
            break;

        case AudioFormat::k_adpcm:
            m_decoder = new (m_mem) AdpcmDecoder(source);
            break;

        default:
            CK_FAIL("unknown format");
            break;
    }
}


}
