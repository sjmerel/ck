#pragma once

#include "ck/core/platform.h"
#include "ck/core/mutex.h"
#include "ck/audio/audiostream.h"

#ifdef __OBJC__
#  define CK_DECLARE @class
#else
#  define CK_DECLARE class
#endif

CK_DECLARE AVURLAsset;
CK_DECLARE AVAssetReader;
CK_DECLARE AVAssetReaderOutput;

struct opaqueCMSampleBuffer;
typedef opaqueCMSampleBuffer* CMSampleBufferRef;

namespace Cki
{


class AssetAudioStream : public AudioStream
{
public:
    AssetAudioStream(const char* url);
    virtual ~AssetAudioStream();

    virtual void init();
    virtual bool isFailed() const;
    virtual int read(void* buf, int blocks);
    virtual int getNumBlocks() const;
    virtual void setBlockPos(int block);
    virtual int getBlockPos() const;

private:
    AVURLAsset* m_asset;
    AVAssetReader* m_reader;
    AVAssetReaderOutput* m_trackOutput;
    CMSampleBufferRef m_sampleBuffer;
    int m_bufferPos; // blocks into sample buffer of read position
    int m_readPos; // blocks into file
    int m_nextReadPos;
    bool m_inited;
    bool m_failed;
    bool m_retried;
    Mutex m_mutex;

    bool createReader(int blockPos);
    void destroyReader();
};


}
