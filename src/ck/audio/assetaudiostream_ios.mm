#include "ck/audio/assetaudiostream_ios.h"
#include "ck/audio/audioformat.h"
#include "ck/core/logger.h"
#include "ck/core/math.h"
#include "ck/core/mem.h"
#include "ck/core/mutexlock.h"

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>


namespace Cki
{


AssetAudioStream::AssetAudioStream(const char* url) :
    m_reader(nil),
    m_trackOutput(nil),
    m_sampleBuffer(NULL),
    m_bufferPos(0),
    m_readPos(0),
    m_nextReadPos(-1),
    m_inited(false),
    m_failed(false),
    m_retried(false)
{
    NSURL* nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url]];
    NSDictionary* options = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:AVURLAssetPreferPreciseDurationAndTimingKey];
    m_asset = [[AVURLAsset alloc] initWithURL:nsurl options:options];

    if (m_asset.hasProtectedContent)
    {
        m_failed = true;
    }
}

AssetAudioStream::~AssetAudioStream()
{
    [m_asset release];
    destroyReader();
}

void AssetAudioStream::init()
{
    if (m_inited || m_failed)
    {
        return;
    }

    m_inited = createReader(0);
}

bool AssetAudioStream::isFailed() const
{
    return m_failed;
}

int AssetAudioStream::read(void* buf, int blocks)
{
    CK_ASSERT(m_inited && !m_failed);

    MutexLock lock(m_mutex);

    // seek if necessary
    if (m_nextReadPos >= 0)
    {
        if (m_nextReadPos != m_readPos)
        {
            destroyReader();
            createReader(m_nextReadPos);
        }
        m_nextReadPos = -1;
    }

    int blocksRead = 0;

    while (blocksRead < blocks)
    {
        if (!m_sampleBuffer)
        {
            if (m_reader.status == AVAssetReaderStatusReading)
            {
                m_sampleBuffer = [m_trackOutput copyNextSampleBuffer];
            }

            AVAssetReaderStatus status = m_reader.status;
            if (status != AVAssetReaderStatusFailed)
            {
                m_retried = false;
            }

            if (status != AVAssetReaderStatusReading)
            {
                if (status == AVAssetReaderStatusFailed)
                {
                    // this happens when bringing app back from background from some reason; try recreating the reader once only
                    if (!m_retried)
                    {
                        int pos = m_readPos;
                        destroyReader();
                        createReader(pos);
                        m_retried = true;
                        continue;
                    }
                    else
                    {
                        CK_LOG_ERROR("Asset reader failed");
                        m_failed = true;
                    }
                }
                else if (status == AVAssetReaderStatusCompleted)
                {
                    m_sampleInfo.blocks = m_readPos;
                }
                return blocksRead;
            }
        }

        CMBlockBufferRef buffer = CMSampleBufferGetDataBuffer(m_sampleBuffer);

        size_t lengthAtOffset;
        size_t totalLength;
        int16* data;

        size_t offset = m_bufferPos * m_sampleInfo.blockBytes;
        CMBlockBufferGetDataPointer(buffer, offset, &lengthAtOffset, &totalLength, (char**) &data);

        int blocksInBuffer = (int) lengthAtOffset / m_sampleInfo.blockBytes;
        int blocksToCopy = Math::min(blocksInBuffer, blocks - blocksRead);
        Mem::copy((byte*) buf + blocksRead * m_sampleInfo.blockBytes, data, blocksToCopy * m_sampleInfo.blockBytes);
        blocksRead += blocksToCopy;
        m_bufferPos += blocksToCopy;
        m_readPos += blocksToCopy;
        CK_ASSERT(m_bufferPos <= totalLength / m_sampleInfo.blockBytes);
        if (m_bufferPos == totalLength / m_sampleInfo.blockBytes)
        {
            m_bufferPos = 0;
            CFRelease(m_sampleBuffer);
            m_sampleBuffer = NULL;
        }
    }
    return blocksRead;
}

int AssetAudioStream::getNumBlocks() const
{
    CK_ASSERT(m_inited && !m_failed);
    return m_sampleInfo.blocks;
}

void AssetAudioStream::setBlockPos(int block)
{
    CK_ASSERT(m_inited && !m_failed);
    MutexLock lock(m_mutex);
    m_nextReadPos = block;
}

int AssetAudioStream::getBlockPos() const
{
    CK_ASSERT(m_inited && !m_failed);
    return m_readPos;
}

////////////////////////////////////////

bool AssetAudioStream::createReader(int blockPos)
{
    MutexLock lock(m_mutex);

    CK_ASSERT(!m_reader);

    // reader
    NSError* err;
    m_reader = [[AVAssetReader alloc] initWithAsset:m_asset error:&err];
    if (err)
    {
        [m_reader release];
        m_reader = nil;
        m_failed = true;
        CK_LOG_ERROR("Could not initialize asset stream: %s", [[err localizedDescription] UTF8String]);
        return false;
    }

    // track
    AVAssetTrack* track = [[m_asset tracksWithMediaType:AVMediaTypeAudio] objectAtIndex:0];

    // track output
    // XXX on iPod 2g running iOS 4.2, using non-nil readerSettings causes the
    // entire render queue to stop when startReading is called!  Passing in nil
    // prevents this, but then the output is the original compressed data.  Not sure if
    // this is a hardware issue or an iOS version issue.
    // TODO could maybe use AudioConverterFillComplexBuffer() to decode with software?
//    NSMutableDictionary* readerSettings = nil;
    NSDictionary* readerSettings = [NSDictionary dictionaryWithObjectsAndKeys:
        [NSNumber numberWithInt:kAudioFormatLinearPCM], AVFormatIDKey, 
        [NSNumber numberWithInt:16], AVLinearPCMBitDepthKey, 
        [NSNumber numberWithInt:NO], AVLinearPCMIsBigEndianKey, 
        [NSNumber numberWithInt:NO], AVLinearPCMIsFloatKey, 
        [NSNumber numberWithInt:NO], AVLinearPCMIsNonInterleavedKey, 
        nil];
    m_trackOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:track outputSettings:readerSettings];
//    m_trackOutput = [AVAssetReaderAudioMixOutput assetReaderAudioMixOutputWithAudioTracks:tracks audioSettings:readerSettings];
    [m_reader addOutput:m_trackOutput];

    CMAudioFormatDescriptionRef fmt = (CMAudioFormatDescriptionRef) [track.formatDescriptions objectAtIndex:0];
    const AudioStreamBasicDescription* asbd = CMAudioFormatDescriptionGetStreamBasicDescription(fmt);

    m_sampleInfo.format = AudioFormat::k_pcmI16;
    m_sampleInfo.channels = (uint8) asbd->mChannelsPerFrame;
    m_sampleInfo.sampleRate = (uint16) asbd->mSampleRate;
    m_sampleInfo.blocks = Math::round(asbd->mSampleRate * CMTimeGetSeconds(m_asset.duration));
    m_sampleInfo.blockBytes = sizeof(int16) * (uint16) asbd->mChannelsPerFrame;
    m_sampleInfo.blockFrames = 1;

    if (blockPos > 0)
    {
        CMTime start = CMTimeMake(blockPos, (int) asbd->mSampleRate);
        m_reader.timeRange = CMTimeRangeMake(start, kCMTimePositiveInfinity);
        m_readPos = blockPos;
    }

    [m_reader startReading];

    return true;
}

void AssetAudioStream::destroyReader()
{
    MutexLock lock(m_mutex);

    [m_reader release];
    m_reader = nil;

    if (m_sampleBuffer)
    {
        CFRelease(m_sampleBuffer);
        m_sampleBuffer = NULL;
    }
}


}
