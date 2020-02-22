#pragma once

#include "ck/core/platform.h"
#include "ck/core/allocatable.h"
#include "ck/core/types.h"


namespace Cki
{


class AudioWriter : public Allocatable
{
public:
    AudioWriter(bool fixedPoint);
    virtual ~AudioWriter();

    virtual bool isValid() const = 0;
    virtual int write(const float* buf, int samples) = 0;
    virtual void close() = 0;

    int write(const int32* buf, int samples);

private:
    float* m_tmpBuf;

    AudioWriter(const AudioWriter&);
    AudioWriter& operator=(const AudioWriter&);
};



}


