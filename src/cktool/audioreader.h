#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/core/byteorder.h"

namespace Cki
{


class AudioReader
{
public:
    AudioReader();
    virtual ~AudioReader() {}
    virtual bool isValid() const = 0;

    virtual int getBytes() const = 0;
    virtual int getFrames() const = 0;
    virtual int getChannels() const = 0;
    virtual int getBitsPerSample() const = 0;
    virtual int getSampleRate() const = 0; // in Hz

    virtual int getNumMarkers() const = 0;
    virtual void getMarkerInfo(int index, const char** name, int& start, int& end) const = 0;

    virtual int getNumLoops() const = 0;
    virtual void getLoopInfo(int index, const char** name, int& start, int& end, int& count) const = 0;

    // Read audio data; return number of samples read.
    virtual int read(int16*, int samples) = 0;

    virtual bool isAtEnd() const = 0;

    static AudioReader* create(const char* filename);

protected:
    void swapSample(byte* data, int sampleBits);
    int16 convertFloatSample(const byte* data);
    int16 convertIntSample(const byte* data, int sampleBits);
    float m_floatNorm;
};


}
