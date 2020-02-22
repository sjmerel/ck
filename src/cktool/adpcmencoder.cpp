#include "adpcmencoder.h"
#include "ck/audio/adpcm.h"
#include "ck/core/debug.h"
#include "ck/core/mem.h"
#include "ck/core/math.h"


namespace Cki
{


namespace
{
    float meanSqDiff(const int16* buf1, int stride1, const int16* buf2, int stride2, int numSamples)
    {
        float sumSqDiff = 0.0f;

        const int16* input1 = buf1;
        const int16* input1End = input1 + numSamples * stride1;
        const int16* input2 = buf2;
        while (input1 < input1End)
        {
            float diff = (float)(*input1 - *input2);
            sumSqDiff += diff * diff;
            input1 += stride1;
            input2 += stride2;
        }
        return sumSqDiff / numSamples;
    }
}

AdpcmEncoder::AdpcmEncoder(int channels, int samplesPerBlock) :
    Encoder(channels),
    m_samplesPerBlock(samplesPerBlock),
    m_inputFrames(0)
{
    CK_ASSERT(channels > 0 && channels <= 2);

    m_input = Mem::allocArray<int16>(samplesPerBlock * channels);

    const int k_initialDelta = 2048; // TODO calculate best initial delta;
    m_delta[0] = k_initialDelta;
    m_delta[1] = k_initialDelta;

    int encodedSize = Adpcm::getBlockSize(samplesPerBlock);
    m_encoded[0] = (byte*) Mem::alloc(encodedSize);
    m_encoded[1] = (byte*) Mem::alloc(encodedSize);

    m_decoded = Mem::allocArray<int16>(samplesPerBlock);
}

AdpcmEncoder::~AdpcmEncoder()
{
    Mem::free(m_input);
    Mem::free(m_encoded[0]);
    Mem::free(m_encoded[1]);
    Mem::free(m_decoded);
}

int AdpcmEncoder::encode(const int16* inBuf, byte* outBuf, int frames, bool final)
{
    int framesCopied = 0;
    int bytesEncoded = 0;
    while (framesCopied < frames)
    {
        // copy input data
        int framesToCopy = Math::min(m_samplesPerBlock - m_inputFrames, frames - framesCopied);
        Mem::copy(m_input + m_inputFrames * m_channels, inBuf + framesCopied * m_channels, framesToCopy * m_channels * sizeof(int16));
        framesCopied += framesToCopy;

        m_inputFrames += framesToCopy;
        CK_ASSERT(m_inputFrames <= m_samplesPerBlock);
        if (m_inputFrames >= m_samplesPerBlock)
        {
            // we have a full block's worth of input
            bytesEncoded += encode(outBuf + bytesEncoded);
            m_inputFrames = 0;
        }
    }

    if (final && m_inputFrames)
    {
        // fill rest of final buffer with 0s
        Mem::clear(m_input + m_inputFrames * m_channels, (m_samplesPerBlock - m_inputFrames) * m_channels * sizeof(int16));
        bytesEncoded += encode(outBuf + bytesEncoded);
        m_inputFrames = 0;
    }

    return bytesEncoded;
}

int AdpcmEncoder::getFramesPerBlock() const
{
    return Adpcm::k_samplesPerBlock;
}

int AdpcmEncoder::getBytesPerBlock() const
{
    return Adpcm::k_bytesPerBlock * m_channels;
}

////////////////////////////////////////

int AdpcmEncoder::findBestEncoding(byte* outBuf, int channel)
{
    // encode block with each predictor value to find the one with the lowest error
    int bytesEncoded = 0;

    float bestErr = -1.0f;
//    int bestPredictor = -1;
    int bestDelta = -1;
    int bestEncoded = 0;
    for (int predictor = 0; predictor < Adpcm::getNumPredictors(); ++predictor)
    {
        int delta = m_delta[channel];
        byte* encoded = m_encoded[1-bestEncoded];
        bytesEncoded = Adpcm::encode(m_input + channel, m_samplesPerBlock, m_channels, encoded, predictor, delta);
        Adpcm::decode(encoded, bytesEncoded, m_decoded, 1);
        float err = meanSqDiff(m_input + channel, m_channels, m_decoded, 1, m_samplesPerBlock);
        if (bestErr < 0.0f || err < bestErr)
        {
            bestErr = err;
            bestDelta = delta;
//            bestPredictor = predictor;
            bestEncoded = 1 - bestEncoded;
        }
    }

    m_delta[channel] = bestDelta;
    Mem::copy(outBuf, m_encoded[bestEncoded], bytesEncoded);

    return bytesEncoded;
}

int AdpcmEncoder::encode(byte* outBuf)
{
    int bytesEncoded = findBestEncoding(outBuf, 0);
    if (m_channels == 2)
    {
        bytesEncoded += findBestEncoding(outBuf + bytesEncoded, 1);
    }
    return bytesEncoded;
}


}
