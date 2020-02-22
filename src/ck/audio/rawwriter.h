#pragma once

#include "ck/core/platform.h"
#include "ck/core/filestream.h"
#include "ck/audio/audiowriter.h"


namespace Cki
{


class RawWriter : public AudioWriter
{
public:
    RawWriter(const char* path, bool fixedPoint);
    virtual ~RawWriter();

    virtual bool isValid() const;
    virtual int write(const float* buf, int samples);
    virtual void close();

private:
    FileStream m_file;

    RawWriter(const RawWriter&);
    RawWriter& operator=(const RawWriter&);
};



}

