#pragma once

#include "ck/core/platform.h"
#include "ck/audio/audiostream.h"

class CkCustomStream;

namespace Cki
{


class CustomAudioStream : public AudioStream
{
public:
    CustomAudioStream(CkCustomStream*);
    virtual ~CustomAudioStream();

    virtual void init();
    virtual bool isFailed() const;
    virtual int read(void* buf, int blocks);
    virtual int getNumBlocks() const;
    virtual void setBlockPos(int block);
    virtual int getBlockPos() const;

private:
    CkCustomStream* m_stream;
    bool m_inited;
    bool m_failed;
};


}


