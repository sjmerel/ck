#pragma once

#include "ck/core/platform.h"
#include "ck/audio/audiostream.h"
#include "ck/pathtype.h"
#include "ck/core/readstream.h"
#include "ck/core/substream.h"
#include "tremor/ivorbisfile.h"

namespace Cki
{


class VorbisAudioStream : public AudioStream
{
public:
    VorbisAudioStream(const char* path, CkPathType, int offset, int length);
    virtual ~VorbisAudioStream();

    virtual void init();
    virtual bool isFailed() const;

    virtual int read(void* buf, int blocks);
    virtual int getNumBlocks() const;
    virtual void setBlockPos(int block);
    virtual int getBlockPos() const;

private:
    ReadStream m_readStream;
    SubStream m_subStream;
    OggVorbis_File m_ovfile;
    bool m_failed;
    bool m_inited;
};


}
