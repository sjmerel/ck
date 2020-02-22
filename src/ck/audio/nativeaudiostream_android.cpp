#include "ck/audio/nativeaudiostream_android.h"
#include "ck/audio/opensles_android.h"
#include "ck/audio/audio_android.h"
#include "ck/audio/audionode.h"
#include "ck/audio/audiodebug_android.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"
#include "ck/audio/audioformat.h"
#include "ck/core/assetmanager_android.h"
#include "ck/core/mem.h"
#include "ck/core/mutexlock.h"
#include "ck/core/thread.h"

namespace Cki
{


NativeAudioStream::NativeAudioStream(const char* path, bool isAsset) :
    m_playerObj(NULL),
    m_playerBufferQueue(NULL),
    m_playerMetadata(NULL),
    m_playerPlay(NULL),
    m_playerSeek(NULL),
    m_playerPrefetchStatus(NULL),
    m_uri(path),
    m_isAsset(isAsset),
    m_inited(false),
    m_failed(false),
    m_end(false),
    m_bufSamples(0),
    m_buf(NULL),
    m_read(NULL),
    m_stored(0),
    m_writeBuf(0),
    m_readFrame(0),
    m_writeFrame(0)
{
    const float bufferMs = 100.0f; // XXX
    int bufFrames = (int) (AudioNode::k_maxSampleRate * bufferMs * 0.001f);
    m_bufSamples = bufFrames * AudioNode::k_maxChannels;

    // TODO alignment?
    m_buf = (int16*) Mem::alloc(m_bufSamples * k_numBufs * sizeof(int16));
    for (int i = 0; i < k_numBufs; ++i)
    {
        m_bufs[i] = m_buf + i*m_bufSamples;
    }
}

NativeAudioStream::~NativeAudioStream()
{
    if (m_playerObj)
    {
        (*m_playerObj)->Destroy(m_playerObj);
    }
    Mem::free(m_buf);
}

void NativeAudioStream::init()
{
    if (!m_failed && !m_inited)
    {
        // source data locator
        SLDataLocator_AndroidFD fdLocator = { 0 };
        SLDataLocator_URI uriLocator = { 0 };

        if (m_isAsset)
        {
            // open asset as file descriptor
            byte assetBuf[AssetManager::k_assetSize];
            AssetManager::AAsset* asset = AssetManager::open(m_uri.getBuffer(), AssetManager::AASSET_MODE_STREAMING, assetBuf);
            if (!asset)
            {
                m_failed = true;
                return;
            }
            off_t start, length;
            int fd = AssetManager::openFileDescriptor(asset, &start, &length);
            // TODO: do we need to close this fd?  close() will fail on it.
            AssetManager::close(asset);

            if (fd < 0)
            {
                m_failed = true;
                return;
            }

            fdLocator.locatorType = SL_DATALOCATOR_ANDROIDFD;
            fdLocator.fd = fd;
            fdLocator.offset = start;
            fdLocator.length = length;
        }
        else
        {
            uriLocator.locatorType = SL_DATALOCATOR_URI;
            uriLocator.URI = (SLchar*) m_uri.getBuffer();
        }

        SLDataFormat_MIME mimeFormat = { 0 };
        mimeFormat.formatType = SL_DATAFORMAT_MIME;
        mimeFormat.mimeType = NULL;
        mimeFormat.containerType = SL_CONTAINERTYPE_UNSPECIFIED;

        // audio source
        SLDataSource audioSrc = { 0 };
        if (m_isAsset)
        {
            audioSrc.pLocator = &fdLocator;
        }
        else
        {
            audioSrc.pLocator = &uriLocator;
        }
        audioSrc.pFormat = &mimeFormat;

        // sink data locator
        SLDataLocator_AndroidSimpleBufferQueue outLocator = { 0 };
        outLocator.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
        outLocator.numBuffers = 2; // ?

        m_sampleInfo.reset();

        // pcm format
        SLDataFormat_PCM pcmFormat = { 0 };
        pcmFormat.formatType = SL_DATAFORMAT_PCM;

        // rest of struct is ignored but must be valid:
        pcmFormat.numChannels = 2;
        pcmFormat.samplesPerSec = SL_SAMPLINGRATE_48;
        pcmFormat.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
        pcmFormat.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
        pcmFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
        pcmFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;

        // audio sink
        SLDataSink audioSnk = { 0 };
        audioSnk.pLocator = &outLocator;
        audioSnk.pFormat = &pcmFormat;

        // create audio player
        const SLInterfaceID ids[] =
        { 
            OpenSLES::SL_IID_BUFFERQUEUE, 
            OpenSLES::SL_IID_SEEK, 
            OpenSLES::SL_IID_PREFETCHSTATUS, 
            OpenSLES::SL_IID_METADATAEXTRACTION,
        };
        const SLboolean req[] = 
        { 
            SL_BOOLEAN_TRUE,
            SL_BOOLEAN_TRUE,
            SL_BOOLEAN_TRUE,
            SL_BOOLEAN_TRUE
        };
        CK_SL_VERIFY( (*Audio::g_engine)->CreateAudioPlayer(Audio::g_engine, &m_playerObj, &audioSrc, &audioSnk, CK_ARRAY_COUNT(ids), ids, req) );

        // NOTE this is slow (10ms sometimes); better to do in separate thread?
        //    CK_SL_VERIFY( (*m_playerObj)->RegisterCallback(m_playerObj, realizeDoneCallback, this) );
        CK_SL_VERIFY( (*m_playerObj)->Realize(m_playerObj, SL_BOOLEAN_FALSE) );

        // get buffer queue interface
        CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_BUFFERQUEUE, &m_playerBufferQueue) );

        // register callback on the buffer queue
        CK_SL_VERIFY( (*m_playerBufferQueue)->RegisterCallback(m_playerBufferQueue, bufferDoneCallback, this) );

        // get play interface
        CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_PLAY, &m_playerPlay) );

        // get seek interface
        CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_SEEK, &m_playerSeek) );

        // get prefetch status interface
        CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_PREFETCHSTATUS, &m_playerPrefetchStatus) );

        // register callback to be notified when playback finishes
        CK_SL_VERIFY( (*m_playerPlay)->RegisterCallback(m_playerPlay, playStatusCallback, this) );
        CK_SL_VERIFY( (*m_playerPlay)->SetCallbackEventsMask(m_playerPlay, SL_PLAYEVENT_HEADATEND) );

        // get metadata extraction interface
        CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_METADATAEXTRACTION, &m_playerMetadata) );

        // register callback to be notified when we're ready to play and to detect errors
        CK_SL_VERIFY( (*m_playerPrefetchStatus)->SetCallbackEventsMask(m_playerPrefetchStatus, SL_PREFETCHEVENT_STATUSCHANGE | SL_PREFETCHEVENT_FILLLEVELCHANGE) );
        CK_SL_VERIFY( (*m_playerPrefetchStatus)->RegisterCallback(m_playerPrefetchStatus, prefetchStatusCallback, this) );

        // enqueue
        m_writeBuf = 0;
        m_read = m_buf;
        enqueue(m_writeBuf);
        setPaused(false);

        while (!m_inited && !m_failed)
        {
            Thread::sleepMs(10);
        }
    }
}

bool NativeAudioStream::isFailed() const
{
    return m_failed;
}

int NativeAudioStream::read(void* buf, int blocks)
{
    MutexLock lock(m_mutex);

    CK_ASSERT(m_inited && !isFailed());

    int16* bufEnd = m_buf + m_bufSamples * k_numBufs;

    int samples = blocks * m_sampleInfo.channels;
    int samplesRead = 0;
    int16* out = (int16*) buf;
    int16* write = m_bufs[m_writeBuf];
    while (samplesRead < samples)
    {
        int samplesToCopy = 0;
        if (m_stored > 0)
        {
            if (m_read < write)
            {
                samplesToCopy = write - m_read;
            }
            else
            {
                // just copy to end of buffer on this iteration
                samplesToCopy = bufEnd - m_read;
            }
            samplesToCopy = Math::min(samplesToCopy, samples - samplesRead);
        }

        if (samplesToCopy > 0)
        {
            Mem::copy(out, m_read, samplesToCopy * sizeof(int16));

            samplesRead += samplesToCopy;
            CK_ASSERT(samplesRead <= samples);

            out += samplesToCopy;
            m_stored -= samplesToCopy;
            CK_ASSERT(m_stored >= 0);
            m_read += samplesToCopy;
            CK_ASSERT(m_read <= bufEnd);
            m_readFrame += samplesToCopy / m_sampleInfo.channels;
            if (m_read == bufEnd)
            {
                m_read = m_buf;
            }
        }
        else 
        {
            if (!m_end)
            {
                // not enough data; decode some more
                m_mutex.unlock();
                setPaused(false);
                Thread::sleepMs(2);
                m_mutex.lock();
                write = m_bufs[m_writeBuf];
            }
            else
            {
                break;
            }
        }
    }

    if (!m_end)
    {
        // decode more if there's space for it
        int freeSamples = m_stored - (m_bufSamples * k_numBufs);
        if (freeSamples >= m_bufSamples)
        {
            setPaused(false);
        }
    }

    int blocksRead = samplesRead / m_sampleInfo.channels;
    return blocksRead;
}

int NativeAudioStream::getNumBlocks() const
{
    CK_ASSERT(m_inited && !isFailed());
    return m_sampleInfo.blocks;
}

void NativeAudioStream::setBlockPos(int block)
{
    SLmillisecond ms = Math::round(1000.0f * block / m_sampleInfo.sampleRate);
    CK_SL_VERIFY( (*m_playerSeek)->SetPosition(m_playerSeek, ms, SL_SEEKMODE_ACCURATE) );
    m_end = false;
}

int NativeAudioStream::getBlockPos() const
{
    CK_ASSERT(m_inited && !isFailed());
    SLmillisecond ms;
    CK_SL_VERIFY( (*m_playerPlay)->GetPosition(m_playerPlay, &ms) );
    return Math::round(ms * 0.001f * m_sampleInfo.sampleRate);
}

////////////////////////////////////////

void NativeAudioStream::playStatusCallback(SLPlayItf, void* context, SLuint32 event)
{
    NativeAudioStream* sound = (NativeAudioStream*) context;
    sound->playStatus(event);
}

void NativeAudioStream::playStatus(SLuint32 event)
{
    MutexLock lock(m_mutex);

    if (event & SL_PLAYEVENT_HEADATEND)
    {
        m_end = true;
        setPaused(true);
    }
}


void NativeAudioStream::bufferDoneCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    NativeAudioStream* me = (NativeAudioStream*) context;
    me->bufferDone(bq);
}

void NativeAudioStream::bufferDone(SLAndroidSimpleBufferQueueItf)
{
    MutexLock lock(m_mutex);

    if (!m_inited)
    {
        if (getMetadata())
        {
            m_inited = true;
        }
        else
        {
            m_failed = true;
            setPaused(true);
            return;
        }
    }

    m_stored += m_bufSamples;
    m_writeFrame += m_bufSamples / m_sampleInfo.channels;

    // queue next buffer to be written to
    m_writeBuf = (m_writeBuf + 1) % k_numBufs;
    enqueue(m_writeBuf);

    int freeSamples = (m_bufSamples * k_numBufs) - m_stored;
    if (freeSamples < m_bufSamples)
    {
        // this is the last free buffer; pause
        setPaused(true);
    }
}

void NativeAudioStream::prefetchStatusCallback(SLPrefetchStatusItf, void* context, SLuint32 event)
{
    NativeAudioStream* sound = (NativeAudioStream*) context;
    sound->prefetchStatus(event);
}

void NativeAudioStream::prefetchStatus(SLuint32 event)
{
    if ((event & SL_PREFETCHEVENT_STATUSCHANGE) && (event & SL_PREFETCHEVENT_FILLLEVELCHANGE))
    {
        // this is the weird way of notifying us of errors, specified in the OpenSL for Android docs
        SLuint32 status;
        CK_SL_VERIFY( (*m_playerPrefetchStatus)->GetPrefetchStatus(m_playerPrefetchStatus, &status) );

        SLpermille level;
        CK_SL_VERIFY( (*m_playerPrefetchStatus)->GetFillLevel(m_playerPrefetchStatus, &level) );
        if (level == 0 && status == SL_PREFETCHSTATUS_UNDERFLOW)
        {
            m_failed = true;
        }
    }
}


bool NativeAudioStream::getMetadata()
{
    SLuint32 numItems;
    CK_SL_VERIFY( (*m_playerMetadata)->GetItemCount(m_playerMetadata, &numItems) );

    uint32 channels = 0;
    uint32 sampleRate = 0;
    uint32 sampleBits = 0;
    uint32 containerBits = 0;
    bool bigEndian = false;

    for (int i = 0; i < numItems; ++i)
    {
        SLuint32 keySize;
        CK_SL_VERIFY( (*m_playerMetadata)->GetKeySize(m_playerMetadata, i, &keySize) );

        char keyBuf[128];
        CK_ASSERT(sizeof(keyBuf) >= keySize);
        SLMetadataInfo* pKey = (SLMetadataInfo*) keyBuf;

        CK_SL_VERIFY( (*m_playerMetadata)->GetKey(m_playerMetadata, i, keySize, pKey) );

        char* keyName = (char*) pKey->data;
        if (String::equals(keyName, ANDROID_KEY_PCMFORMAT_NUMCHANNELS))
        {
            channels = getMetadataValue(i);
        }
        else if (String::equals(keyName, ANDROID_KEY_PCMFORMAT_SAMPLERATE))
        {
            sampleRate = getMetadataValue(i);
        }
        else if (String::equals(keyName, ANDROID_KEY_PCMFORMAT_BITSPERSAMPLE))
        {
            sampleBits = getMetadataValue(i);
        }
        else if (String::equals(keyName, ANDROID_KEY_PCMFORMAT_CONTAINERSIZE))
        {
            containerBits = getMetadataValue(i);
        }
        else if (String::equals(keyName, ANDROID_KEY_PCMFORMAT_CHANNELMASK))
        {
            // don't care
        }
        else if (String::equals(keyName, ANDROID_KEY_PCMFORMAT_ENDIANNESS))
        {
            bigEndian = (getMetadataValue(i) == SL_BYTEORDER_BIGENDIAN);
        }
    }

    if (channels != 1 && channels != 2)
    {
        CK_LOG_ERROR("Unsupported number of channels: %d", channels);
        return false;
    }
    if (sampleRate > AudioNode::k_maxSampleRate)
    {
        CK_LOG_ERROR("Unsupported sample rate: %d", sampleRate);
        return false;
    }
    if (sampleBits != 16 && sampleBits != 8)
    {
        CK_LOG_ERROR("Unsupported sample size: %d bits", sampleBits);
        return false;
    }
    if (containerBits != 16 && containerBits != 8)
    {
        CK_LOG_ERROR("Unsupported sample container size: %d bits", containerBits);
        return false;
    }
    if (containerBits != sampleBits)
    {
        CK_LOG_ERROR("Samples bits (%d) different from container bits (%d)",
                sampleBits, containerBits);
        return false;
    }
    if (bigEndian)
    {
        CK_LOG_ERROR("Big endian samples not supported");
        return false;
    }

    // TODO handle different sample sizes, container sizes, endian, etc

//    CK_PRINT(" channels: %d\n", channels);
//    CK_PRINT(" sampleRate: %d\n", sampleRate);
//    CK_PRINT(" sampleBits: %d\n", sampleBits);
//    CK_PRINT(" containerBits: %d\n", containerBits);

    SLmillisecond msec;
    CK_SL_VERIFY( (*m_playerPlay)->GetDuration(m_playerPlay, &msec) );

    m_sampleInfo.format = (sampleBits == 16 ? AudioFormat::k_pcmI16 : AudioFormat::k_pcmI8);
    m_sampleInfo.channels = channels;
    m_sampleInfo.sampleRate = sampleRate;
    m_sampleInfo.blocks = Math::round(msec * 0.001f * sampleRate);
    m_sampleInfo.blockBytes = channels * sampleBits / 8;
    m_sampleInfo.blockFrames = 1;

    return true;
}

uint32 NativeAudioStream::getMetadataValue(int i)
{
    SLuint32 valueSize;
    CK_SL_VERIFY( (*m_playerMetadata)->GetValueSize(m_playerMetadata, i, &valueSize) );

    char valueBuf[128];
    CK_ASSERT(sizeof(valueBuf) >= valueSize);
    SLMetadataInfo* pValue = (SLMetadataInfo*) valueBuf;

    CK_SL_VERIFY( (*m_playerMetadata)->GetValue(m_playerMetadata, i, valueSize, pValue) );
    SLuint32* p = (SLuint32*) pValue->data;
    return *p;
}

void NativeAudioStream::enqueue(int index)
{
    CK_ASSERT(index >= 0 && index < k_numBufs);

    CK_SL_VERIFY( (*m_playerBufferQueue)->Enqueue(m_playerBufferQueue, m_bufs[index], m_bufSamples * sizeof(int16)) );
}

void NativeAudioStream::setPaused(bool paused)
{
    CK_SL_VERIFY( (*m_playerPlay)->SetPlayState(m_playerPlay, paused ? SL_PLAYSTATE_PAUSED : SL_PLAYSTATE_PLAYING) );
}

}

