#pragma once

#include "ck/core/platform.h"
#include "ck/core/filestream.h"
#include "ck/core/substream.h"
#include "ck/audio/audiostream.h"
#include <AudioToolbox/AudioToolbox.h>

namespace Cki
{


class NativeAudioStream : public AudioStream
{
public:
    NativeAudioStream(const char* path, int offset, int length);
    virtual ~NativeAudioStream();

    virtual void init();
    virtual bool isFailed() const;
    virtual int read(void* buf, int blocks);
    virtual int getNumBlocks() const; // not always accurate for MP3 files, until we've actually read to the end!
    virtual void setBlockPos(int block);
    virtual int getBlockPos() const;

private:
    AudioFileID m_audioFileId;
    ExtAudioFileRef m_audioFile;
    FileStream m_fileStream;
    SubStream m_subStream;
    int m_tellOffset; // workaround for bug in ExtAudioFileTell() on Mac
    bool m_inited;
    bool m_failed;
    uint32 m_srcFormatId;

    int tell() const;

    static OSStatus readFunc(void*, SInt64, UInt32, void*, UInt32*);
    static SInt64 getSizeFunc(void*);
};


}
