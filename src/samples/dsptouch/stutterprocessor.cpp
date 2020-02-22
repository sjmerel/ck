#include "stutterprocessor.h"
#include <stdio.h>

StutterProcessor::StutterProcessor() :
    m_periodFrames( (int) (CkEffectProcessor::getSampleRate() * 0.1f)), // 100 ms
    m_frames(0)
{
}

void StutterProcessor::setParam(int paramId, float value)
{
    switch (paramId)
    {
        case k_periodMs:
            // convert the period from milliseconds to frames
            m_periodFrames = (int) (CkEffectProcessor::getSampleRate() * value * 0.001f);
            if (m_periodFrames <= 0)
            {
                m_periodFrames = 0;
            }
            break;

        default:
            printf("Stutter effect: unknown param ID %d", paramId);
            break;
    }
}

void StutterProcessor::reset()
{
    m_frames = 0;
}

bool StutterProcessor::isInPlace() const
{
    // Return true because this effect can be processed "in-place", i.e. by
    // altering the data in the input buffer, rather than by writing the
    // processed data into a separate buffer.  This allows some modest memory savings.
    return true;
}

void StutterProcessor::process(int* inBuf, int* outBuf, int frames)
{
    // Note that because isInPlace() returns true, inBuf and outBuf are 
    // equal, and we can just modify inBuf.  Otherwise, we would need to
    // write the new data to outBuf.

    // The input and output buffers contain interleaved stereo data, meaning
    // they alternate between values for the left and right channels.

    // The samples are signed 32-bit fixed-point values with 24 fractional bits.  
    // For this simple example, we can ignore the details of fixed-point math,
    // since we're just setting the samples to zero.

    int* p = inBuf;
    int* pEnd = inBuf + frames * 2; 

    while (p < pEnd)
    {
        if (m_frames > m_periodFrames/2)
        {
            // set left and right channels to 0
            *p++ = 0;
            *p++ = 0;
        }
        else 
        {
            // leave left and right channels as they are
            p += 2;
        }

        ++m_frames;
        if (m_frames >= m_periodFrames)
        {
            m_frames = 0;
        }
    }
}

void StutterProcessor::process(float* inBuf, float* outBuf, int frames)
{
    // Note that because isInPlace() returns true, inBuf and outBuf are 
    // equal, and we can just modify inBuf.  Otherwise, we would need to
    // write the new data to outBuf.

    // The input and output buffers contain interleaved stereo data, meaning
    // they alternate between values for the left and right channels.

    float* p = inBuf;
    float* pEnd = inBuf + frames * 2; 

    while (p < pEnd)
    {
        if (m_frames > m_periodFrames/2)
        {
            // set left and right channels to 0
            *p++ = 0.0f;
            *p++ = 0.0f;
        }
        else 
        {
            // leave left and right channels as they are
            p += 2;
        }

        ++m_frames;
        if (m_frames >= m_periodFrames)
        {
            m_frames = 0;
        }
    }
}

// factory function, registered with CkEffect::registerCustomEffect.
CkEffectProcessor* StutterProcessor::create(void* arg)
{
    return new StutterProcessor();
}
