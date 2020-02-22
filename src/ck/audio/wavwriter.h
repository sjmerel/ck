#pragma once

#include "ck/core/platform.h"
#include "ck/core/filestream.h"
#include "ck/audio/audiowriter.h"


namespace Cki
{


class WavWriter : public AudioWriter
{
public:
    WavWriter(const char* path, int channels, int sampleRate, bool fixedPoint);
    virtual ~WavWriter();

    virtual bool isValid() const;
    virtual int write(const float* buf, int samples);
    virtual void close();

private:
    FileStream m_file;

    WavWriter(const WavWriter&);
    WavWriter& operator=(const WavWriter&);
};



}


