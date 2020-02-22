#pragma once

#include "ck/core/platform.h"
#include "ck/core/fixedstring.h"
#include "ck/core/mutex.h"
#include "ck/audio/audiostream.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace Cki
{


class NativeAudioStream : public AudioStream
{
public:
    NativeAudioStream(const char* path, bool isAsset);
    ~NativeAudioStream();

    virtual void init();
    virtual bool isFailed() const;
    virtual int read(void* buf, int blocks);
    virtual int getNumBlocks() const; // not always accurate for MP3 files, until we've actually read to the end!
    virtual void setBlockPos(int block);
    virtual int getBlockPos() const;

private:
    SLObjectItf m_playerObj;
    SLAndroidSimpleBufferQueueItf m_playerBufferQueue;
    SLMetadataExtractionItf m_playerMetadata;
    SLPlayItf m_playerPlay;
    SLSeekItf m_playerSeek;
    SLPrefetchStatusItf m_playerPrefetchStatus;
    FixedString256 m_uri;
    Mutex m_mutex;
    bool m_isAsset;
    bool m_inited;
    bool m_failed;
    bool m_end;

    enum { k_numBufs = 4 };
    int m_bufSamples;
    int16* m_buf;
    int16* m_read;
    int m_stored;

    int16* m_bufs[k_numBufs]; 
    int m_writeBuf; // index of buffer being written to

    int m_readFrame; // frame position of read
    int m_writeFrame; // frame position of write

    static void bufferDoneCallback(SLAndroidSimpleBufferQueueItf, void *context);
    void bufferDone(SLAndroidSimpleBufferQueueItf);
    static void playStatusCallback(SLPlayItf, void* context, SLuint32 event);
    void playStatus(SLuint32 event);
    static void prefetchStatusCallback(SLPrefetchStatusItf, void* context, SLuint32 event);
    void prefetchStatus(SLuint32 event);

    bool getMetadata();
    uint32 getMetadataValue(int i);
    void enqueue(int);
    void setPaused(bool paused);
};


}

