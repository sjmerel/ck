#include "ck/audio/nativeaudiostream_ios.h"
#include "ck/audio/audiodebug_ios.h"
#include "ck/audio/audioformat.h"
#include "ck/core/debug.h"
#include "ck/core/system.h"
#include "ck/core/logger.h"
#include "ck/core/math.h"
#include "ck/audio/audionode.h"

namespace Cki
{


namespace
{
    // TODO use smaller buffer and multiple reads?
    const int k_padFrames = 1100;
    byte g_padBuf[k_padFrames * 2 * sizeof(int16)]; // enough for k_padFrames of stereo PCM16

    template <typename T>
    void getExtAudioFileProperty(ExtAudioFileRef audioFile, ExtAudioFilePropertyID propertyId, T& value)
    {
        UInt32 size = sizeof(T);
        CK_OSSTATUS_VERIFY( ExtAudioFileGetProperty(audioFile, propertyId, &size, &value) );
        CK_ASSERT(size == sizeof(T));
    }

    template <typename T>
    void setExtAudioFileProperty(ExtAudioFileRef audioFile, ExtAudioFilePropertyID propertyId, const T& value)
    {
        CK_OSSTATUS_VERIFY( ExtAudioFileSetProperty(audioFile, propertyId, sizeof(T), &value) );
    }
}


NativeAudioStream::NativeAudioStream(const char* path, int offset, int length) :
    m_audioFileId(NULL),
    m_audioFile(NULL),
    m_fileStream(path),
    m_subStream(m_fileStream, offset, length),
    m_tellOffset(0),
    m_inited(false),
    m_failed(false)
{
}

NativeAudioStream::~NativeAudioStream()
{
    if (m_audioFile)
    {
        ExtAudioFileDispose(m_audioFile);
        AudioFileClose(m_audioFileId);
    }
}

void NativeAudioStream::init()
{
    if (!m_failed && !m_inited)
    {
        OSStatus result = AudioFileOpenWithCallbacks(this, readFunc, NULL, getSizeFunc, NULL, 0 /* ? */, &m_audioFileId);
        if (result != 0)
        {
            m_failed = true;
            CK_LOG_ERROR("Could not load native stream (error %d)", result);
            return;
        }
        CK_OSSTATUS_VERIFY( ExtAudioFileWrapAudioFileID(m_audioFileId, false, &m_audioFile) );

        m_sampleInfo.reset();
        m_sampleInfo.format = AudioFormat::k_pcmI16;

        // get format of file
        AudioStreamBasicDescription fileFormat = {0};
        getExtAudioFileProperty(m_audioFile, kExtAudioFileProperty_FileDataFormat, fileFormat);
        m_sampleInfo.channels = (uint8) fileFormat.mChannelsPerFrame;
        m_sampleInfo.sampleRate = (uint16) fileFormat.mSampleRate;
        m_srcFormatId = fileFormat.mFormatID;

        // get frames
        UInt64 frameCount = 0;
        getExtAudioFileProperty(m_audioFile, kExtAudioFileProperty_FileLengthFrames, frameCount);
        m_sampleInfo.blocks = (uint32) frameCount;

        m_sampleInfo.blockBytes = sizeof(int16) * m_sampleInfo.channels;
        m_sampleInfo.blockFrames = 1;

#if CK_PLATFORM_IOS || CK_PLATFORM_TVOS
        if (System::get()->getConfig().enableHardwareDecoding)
        {
            // try to use hardware codec
            UInt32 codec = kAppleHardwareAudioCodecManufacturer;
            setExtAudioFileProperty(m_audioFile, kExtAudioFileProperty_CodecManufacturer, codec);
        }
#endif

        // set output format
        // TODO output in 8.24 instead?
        int bytesPerSample = sizeof(int16);
        AudioStreamBasicDescription format = {0};
        format.mFormatID = kAudioFormatLinearPCM;
        format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
        format.mBytesPerPacket = bytesPerSample * m_sampleInfo.channels;
        format.mFramesPerPacket = 1;
        format.mBytesPerFrame = bytesPerSample * m_sampleInfo.channels;
        format.mChannelsPerFrame = m_sampleInfo.channels;
        format.mBitsPerChannel = 8 * bytesPerSample;
        format.mSampleRate = m_sampleInfo.sampleRate;

        result = ExtAudioFileSetProperty(m_audioFile, kExtAudioFileProperty_ClientDataFormat, sizeof(format), &format);
#if CK_PLATFORM_IOS || CK_PLATFORM_TVOS
        if (System::get()->getConfig().enableHardwareDecoding)
        {
            if (result == kAudioConverterErr_HardwareInUse)
            {
                // hardware codec in use; have to close and reopen file for some reason, then set format again
                CK_OSSTATUS_VERIFY( ExtAudioFileDispose(m_audioFile) );
                CK_OSSTATUS_VERIFY( ExtAudioFileWrapAudioFileID(m_audioFileId, false, &m_audioFile) );
                result = ExtAudioFileSetProperty(m_audioFile, kExtAudioFileProperty_ClientDataFormat, sizeof(format), &format);
            }
            else
            {
                // hardware codec not in use, but might fail on first read based on audio session category!
                int16 buf[AudioNode::k_maxChannels];
                AudioBufferList bufList;
                bufList.mNumberBuffers = 1;
                bufList.mBuffers[0].mNumberChannels = m_sampleInfo.channels;
                bufList.mBuffers[0].mDataByteSize = m_sampleInfo.blockBytes;
                bufList.mBuffers[0].mData = buf;

                UInt32 frames = 1;
                result = ExtAudioFileRead(m_audioFile, &frames, &bufList);
                CK_OSSTATUS_VERIFY( ExtAudioFileSeek(m_audioFile, 0) );
                if (result == kAudioQueueErr_InvalidCodecAccess)
                {
                    // hardware codec in use; have to close and reopen file for some reason, then set format again
                    CK_OSSTATUS_VERIFY( ExtAudioFileDispose(m_audioFile) );
                    CK_OSSTATUS_VERIFY( ExtAudioFileWrapAudioFileID(m_audioFileId, false, &m_audioFile) );
                    result = ExtAudioFileSetProperty(m_audioFile, kExtAudioFileProperty_ClientDataFormat, sizeof(format), &format);
                }
            }
        }
#endif
        CK_ASSERT(result == 0);

//        UInt32 codec;
//        getExtAudioFileProperty(m_audioFile, kExtAudioFileProperty_CodecManufacturer, codec);
//        CK_PRINT("codec: %.4s\n", &codec);

        m_sampleInfo.volume = CK_UINT16_MAX;
        m_sampleInfo.pan = 0;
        m_sampleInfo.loopCount = 0;
        m_sampleInfo.loopStart = 0;
        m_sampleInfo.loopEnd = -1;

        // workaround for bug in ExtAudioFileTell on OS X:
        // http://developer.apple.com/library/ios/#qa/qa1678/_index.html
        m_tellOffset = tell();

        m_inited = true;
    }
}

bool NativeAudioStream::isFailed() const
{
    return m_failed;
}

int NativeAudioStream::read(void* buf, int blocks)
{
    CK_ASSERT(m_inited && !isFailed());

    AudioBufferList bufList;
    bufList.mNumberBuffers = 1;
    bufList.mBuffers[0].mNumberChannels = m_sampleInfo.channels;
    bufList.mBuffers[0].mDataByteSize = blocks * m_sampleInfo.blockBytes;
    bufList.mBuffers[0].mData = buf;

    int framesToRead = blocks;
    UInt32 frames = framesToRead;
    CK_OSSTATUS_VERIFY( ExtAudioFileRead(m_audioFile, &frames, &bufList) );
    CK_ASSERT(frames <= framesToRead);
    if (frames < framesToRead)
    {
        // file is shorter than we thought!
        // This seems to happen for some mp3 files; I think the length stored in the file is an estimate,
        // and the actual length can vary based on the decoder.
        // Also, iOS 4.1-4.2 actually can play further the first time through than subsequent repeats!
        m_sampleInfo.blocks = tell();
    }
    return frames;
}

int NativeAudioStream::getNumBlocks() const
{
    CK_ASSERT(m_inited && !isFailed());
    return m_sampleInfo.blocks;
}

void NativeAudioStream::setBlockPos(int block)
{
    if (block != tell())
    {
        CK_ASSERT(m_inited && !isFailed());

        if (m_srcFormatId == kAudioFormatMPEGLayer3 && block > 0)
        {
            // After a seek in an MP3 file, I've noticed artifacts (looks like ringing)
            // in decoded data.  So seek early and decode and throw away about 1100 frames,
            // which seems to fix it.  (Doesn't seem to happen in M4A files.)
            int padFrames = Math::min(k_padFrames, block);

            AudioBufferList bufList;
            bufList.mNumberBuffers = 1;
            bufList.mBuffers[0].mNumberChannels = m_sampleInfo.channels;
            bufList.mBuffers[0].mDataByteSize = padFrames * m_sampleInfo.blockBytes;
            bufList.mBuffers[0].mData = g_padBuf;

            CK_OSSTATUS_VERIFY( ExtAudioFileSeek(m_audioFile, block - padFrames) );
            UInt32 frames = padFrames;
            CK_OSSTATUS_VERIFY( ExtAudioFileRead(m_audioFile, &frames, &bufList) );
            //CK_ASSERT(frames == padFrames);
        }
        else
        {
            CK_OSSTATUS_VERIFY( ExtAudioFileSeek(m_audioFile, block) );
        }
    }
}

int NativeAudioStream::getBlockPos() const
{
    CK_ASSERT(m_inited && !isFailed());
    return tell();
}

////////////////////////////////////////

int NativeAudioStream::tell() const
{
    // return data position in frames, accounting for Mac OS offset bug
    SInt64 pos;
    CK_OSSTATUS_VERIFY( ExtAudioFileTell(m_audioFile, &pos) );
    return (int) pos - m_tellOffset;
}

OSStatus NativeAudioStream::readFunc(void* data, SInt64 inPosition, UInt32 requestCount, void* buffer, UInt32* actualCount)
{
    NativeAudioStream* me = (NativeAudioStream*) data;
    me->m_subStream.setPos((int) inPosition);
    *actualCount = me->m_subStream.read(buffer, requestCount);
    return 0;
}

SInt64 NativeAudioStream::getSizeFunc(void* data)
{
    NativeAudioStream* me = (NativeAudioStream*) data;
    return me->m_subStream.getSize();
}

}
