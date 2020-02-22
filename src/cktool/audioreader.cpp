#include "cktool/audioreader.h"
#include "cktool/wavreader.h"
#include "cktool/aiffreader.h"
#include "ck/core/debug.h"
#include "ck/core/mem.h"

namespace Cki
{


AudioReader::AudioReader() : m_floatNorm(1.0f) {}

AudioReader* AudioReader::create(const char* filename)
{
    WavReader* wavReader = new WavReader(filename);
    if (wavReader->isValid())
    {
        return wavReader;
    }
    else
    {
        delete wavReader;
    }

    AiffReader* aiffReader = new AiffReader(filename);
    if (aiffReader->isValid())
    {
        return aiffReader;
    }
    else
    {
        delete aiffReader;
    }

    return NULL;
}

void AudioReader::swapSample(byte* data, int sampleBits)
{
    byte tmp;
    switch (sampleBits)
    {
        case 8: 
            break;

        case 16: 
            tmp = data[0];
            data[0] = data[1];
            data[1] = tmp;
            break;

        case 24: 
            tmp = data[0];
            data[0] = data[2];
            data[2] = tmp;
            break;

        case 32: 
            tmp = data[0];
            data[0] = data[3];
            data[3] = tmp;

            tmp = data[1];
            data[1] = data[2];
            data[2] = tmp;

            break;

        default:
            CK_FAIL("illegal sample size");
    }
}

int16 AudioReader::convertFloatSample(const byte* data)
{
    float tmp;
    Mem::copy(&tmp, data, sizeof(tmp));
    return (int16) (tmp / m_floatNorm * CK_INT16_MAX);
}

int16 AudioReader::convertIntSample(const byte* data, int sampleBits)
{
    int16 value = 0;
    switch (sampleBits)
    {
        case 8:
            value = data[0] << 8;
            break;

        case 16:
            value = data[0] | (data[1] << 8);
            break;

        case 24:
            value = data[1] | (data[2] << 8);
            break;

        case 32:
            value = data[2] | (data[3] << 8);
            break;

        default:
            CK_FAIL("illegal block size");
    }

    return value;
}


}
