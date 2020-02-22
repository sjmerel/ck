#pragma once

#include "ck/core/platform.h"
#include "ck/pathtype.h"
#include "ck/audio/audiostream.h"
#include "ck/core/readstream.h"
#include "ck/core/substream.h"

namespace Cki
{


// Audio stream for .cks files

class CksAudioStream : public AudioStream
{
public:
    CksAudioStream(const char* path, CkPathType, int offset, int length);

    virtual void init();
    virtual bool isFailed() const;
    virtual int read(void* buf, int blocks);
    virtual int getNumBlocks() const;
    virtual void setBlockPos(int block);
    virtual int getBlockPos() const;

private:
    int m_dataPos; // position of start of audio data, or -1 if not initialized
    bool m_failed;
    ReadStream m_readStream;
    SubStream m_subStream;
};


}


