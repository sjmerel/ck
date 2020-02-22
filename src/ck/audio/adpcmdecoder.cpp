#include "ck/audio/adpcmdecoder.h"
#include "ck/audio/audiosource.h"
#include "ck/audio/bufferhandle.h"
#include "ck/audio/audionode.h"
#include "ck/audio/audioutil.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/mem.h"


namespace Cki
{


AdpcmDecoder::AdpcmDecoder(AudioSource& source) :
    Decoder(source),
    m_decodedBlock(0),
    m_frameOffset(0),
    m_valid(false)
{
}

int AdpcmDecoder::decode(int32* buf, int frames)
{
    return decodeImpl(buf, frames);
}

int AdpcmDecoder::decode(float* buf, int frames)
{
    return decodeImpl(buf, frames);
}

template <typename T> 
int AdpcmDecoder::decodeImpl(T* buf, int frames)
{
    if (isDone())
    {
        return 0;
    }
    else
    {
        int channels = m_source.getSampleInfo().channels;

        int framesCopied = 0;
        T* out = buf;
        while (framesCopied < frames && !isDone())
        {
            if (!m_valid)
            {
                if (!decodeBlock())
                {
                    break;
                }
                m_valid = true;
            }

            int framesToCopy = Math::min(frames - framesCopied, Adpcm::k_samplesPerBlock - m_frameOffset);
            int bytesToCopy = framesToCopy * channels * sizeof(int16);
            Mem::copy((int16*) out, &m_decoded[m_frameOffset * channels], bytesToCopy);
            AudioUtil::convert((int16*) out, out, framesToCopy * channels);
            out += framesToCopy * channels;

            m_frameOffset += framesToCopy;
            CK_ASSERT(m_frameOffset <= Adpcm::k_samplesPerBlock);
            if (m_frameOffset >= Adpcm::k_samplesPerBlock)
            {
                m_frameOffset = 0;
                m_valid = false;
            }

            framesCopied += framesToCopy;
            CK_ASSERT(framesCopied <= frames);
        }

        return framesCopied;
    }
}

bool AdpcmDecoder::isDone() const
{
    return m_source.isDone() && !m_valid;
}

void AdpcmDecoder::setFramePos(int frame)
{
    // TODO don't need to re-decode, if in same block
    // seek to beginning of block
    int block = frame / Adpcm::k_samplesPerBlock;
    m_source.setBlockPos(block);
    m_valid = false;
    m_frameOffset = frame - (block * Adpcm::k_samplesPerBlock);
}

int AdpcmDecoder::getFramePos() const
{
    if (m_valid)
    {
        return m_decodedBlock * Adpcm::k_samplesPerBlock + m_frameOffset;
    }
    else
    {
        return m_source.getBlockPos() * Adpcm::k_samplesPerBlock + m_frameOffset;
    }
}

int AdpcmDecoder::getNumFrames() const
{
    return m_source.getNumBlocks() * Adpcm::k_samplesPerBlock;
}

////////////////////////////////////////

// decode next block
bool AdpcmDecoder::decodeBlock()
{
    int channels = m_source.getSampleInfo().channels;
    int decodedBlock = m_source.getBlockPos();

    // read block of encoded data (all channels)
    byte encoded[Adpcm::k_bytesPerBlock * 2];
    int blocksRead = m_source.read(encoded, 1);
    CK_ASSERT(blocksRead == 1 || blocksRead == 0);

    if (blocksRead == 0)
    {
        return false;
    }
    else
    {
        // decode
        Adpcm::decode(encoded, Adpcm::k_bytesPerBlock, m_decoded, channels);
        if (channels == 2)
        {
            Adpcm::decode(encoded + Adpcm::k_bytesPerBlock, Adpcm::k_bytesPerBlock, m_decoded + 1, channels);
        }
        m_decodedBlock = decodedBlock;
        return true;
    }
}




}
