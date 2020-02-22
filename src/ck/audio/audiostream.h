#pragma once

#include "ck/core/platform.h"
#include "ck/audio/sampleinfo.h"
#include "ck/core/debug.h"

namespace Cki
{


struct SampleInfo;

class AudioStream
{
public:
    AudioStream();
    virtual ~AudioStream() {}

    // read file header to get sample info
    virtual void init() = 0;
    virtual bool isFailed() const = 0;

    const SampleInfo& getSampleInfo() const { return m_sampleInfo; }

    // the following functions behave as if the file contains only
    // audio data as described by the SampleInfo struct
    virtual int read(void* buf, int blocks) = 0;
    virtual int getNumBlocks() const = 0; // may return -1 if not known
    virtual void setBlockPos(int block) = 0;
    virtual int getBlockPos() const = 0;

protected:
    SampleInfo m_sampleInfo;
};


}

