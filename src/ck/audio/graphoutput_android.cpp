#include "ck/audio/graphoutput_android.h"
#include "ck/audio/audio_android.h"
#include "ck/audio/audiodebug_android.h"
#include "ck/audio/opensles_android.h"
#include "ck/audio/audioutil.h"
#include "ck/core/debug.h"
#include "ck/core/system.h"
#include "ck/core/mem.h"

namespace
{
    const float k_primingBufMs = 300.0f; // TODO use AudioTrack.getMinBufferSize() to calculate this?
    const int k_primingBufSize = (int) (Cki::AudioNode::k_maxSampleRate * k_primingBufMs * 0.001f * Cki::AudioNode::k_maxChannels);
}

namespace Cki
{

GraphOutputAndroid::GraphOutputAndroid() :
    m_playerObj(NULL),
    m_playerBufferQueue(NULL),
    m_playerPlay(NULL),
    m_playerVolume(NULL),
    m_framesPerBuffer(0),
    m_curBuf(0)
{
    int sampleRate = Audio::getNativeSampleRate();

    // allocate audio buffers
    float bufferMs = System::get()->getConfig().audioUpdateMs;
    m_framesPerBuffer = (int) (sampleRate * bufferMs * 0.001f);
    int bytesPerBuffer = m_framesPerBuffer * sizeof(int16) * AudioNode::k_maxChannels;
    // aligning to 16 bytes for SSE
    // TODO statically allocate?
    m_bufs[0] = (int16*) Mem::alloc(bytesPerBuffer, 16);
    m_bufs[1] = (int16*) Mem::alloc(bytesPerBuffer, 16);

    m_renderBuf = (int32*) Mem::alloc(m_framesPerBuffer * sizeof(int32) * AudioNode::k_maxChannels, 16);

    // allocate "priming" buffer
    m_primingBuf = (int16*) Mem::alloc(k_primingBufSize);
    Mem::clear(m_primingBuf, k_primingBufSize);

    // source data locator
    SLDataLocator_AndroidSimpleBufferQueue bufLocator = { 0 };
    bufLocator.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
    bufLocator.numBuffers = 2;

    // pcm format
    // TODO: in Android L, output float samples directly
    SLDataFormat_PCM pcmFormat = { 0 };
    pcmFormat.formatType = SL_DATAFORMAT_PCM;
    pcmFormat.numChannels = AudioNode::k_maxChannels;
    pcmFormat.samplesPerSec = sampleRate * 1000;
    pcmFormat.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    pcmFormat.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    pcmFormat.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    pcmFormat.endianness = SL_BYTEORDER_LITTLEENDIAN;

    // audio source
    SLDataSource audioSrc = { 0 };
    audioSrc.pLocator = &bufLocator;
    audioSrc.pFormat = &pcmFormat;

    // sink data locator
    SLDataLocator_OutputMix outLocator = { 0 };
    outLocator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    outLocator.outputMix = Audio::g_outputMixObj;

    // audio sink
    SLDataSink audioSnk = { 0 };
    audioSnk.pLocator = &outLocator;
    audioSnk.pFormat = NULL;

    // create audio player
    const SLInterfaceID ids[] = { OpenSLES::SL_IID_BUFFERQUEUE, OpenSLES::SL_IID_VOLUME };
    const SLboolean req[] =     { SL_BOOLEAN_TRUE,              SL_BOOLEAN_TRUE };
    CK_SL_VERIFY( (*Audio::g_engine)->CreateAudioPlayer(Audio::g_engine, &m_playerObj, &audioSrc, &audioSnk, CK_ARRAY_COUNT(ids), ids, req) );

    // NOTE this is slow (10ms sometimes); better to do in separate thread?
    CK_SL_VERIFY( (*m_playerObj)->Realize(m_playerObj, SL_BOOLEAN_FALSE) );

    // get buffer queue interface
    CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_BUFFERQUEUE, &m_playerBufferQueue) );

    // register callback on the buffer queue
    CK_SL_VERIFY( (*m_playerBufferQueue)->RegisterCallback(m_playerBufferQueue, bufferDoneCallback, this) );

    // get play interface
    CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_PLAY, &m_playerPlay) );

    // get volume interface
    CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_VOLUME, &m_playerVolume) );
}

GraphOutputAndroid::~GraphOutputAndroid()
{
    stop();
    (*m_playerObj)->Destroy(m_playerObj);
    Mem::free(m_bufs[0]);
    Mem::free(m_bufs[1]);
    Mem::free(m_renderBuf);
    Mem::free(m_primingBuf);
}

void GraphOutputAndroid::startImpl()
{
    m_curBuf = 0;

    // First, queue up a large "priming buffer".  
    // (If buffers are small, buffer queue sometimes never starts playing; this seems
    // to get it going. The buffers of actual audio data will be queued up after this.)
    CK_SL_VERIFY( (*m_playerBufferQueue)->Enqueue(m_playerBufferQueue, m_primingBuf, k_primingBufSize) );

//    enqueue();

    CK_SL_VERIFY( (*m_playerPlay)->SetPlayState(m_playerPlay, SL_PLAYSTATE_PLAYING) );

}

void GraphOutputAndroid::stopImpl()
{
    CK_SL_VERIFY( (*m_playerPlay)->SetPlayState(m_playerPlay, SL_PLAYSTATE_STOPPED) );
    CK_SL_VERIFY( (*m_playerBufferQueue)->Clear(m_playerBufferQueue) );
}

void GraphOutputAndroid::bufferDoneCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    GraphOutputAndroid* me = (GraphOutputAndroid*) context;
    me->bufferDone(bq);
}

void GraphOutputAndroid::bufferDone(SLAndroidSimpleBufferQueueItf)
{
    if (isRunning())
    {
        enqueue();
    }
}

void GraphOutputAndroid::enqueue()
{
    // TODO: in Android L, can output float samples directly

    int16* buf = m_bufs[m_curBuf];
    if (System::get()->getSampleType() == kCkSampleType_Float)
    {
        render((float*) m_renderBuf, m_framesPerBuffer);
        AudioUtil::convert((float*) m_renderBuf, buf, m_framesPerBuffer*2);
    }
    else
    {
        render((int32*) m_renderBuf, m_framesPerBuffer);
        AudioUtil::convert((int32*) m_renderBuf, buf, m_framesPerBuffer*2);
    }

    // enqueue
    CK_SL_VERIFY( (*m_playerBufferQueue)->Enqueue(m_playerBufferQueue, buf, m_framesPerBuffer * sizeof(int16) * AudioNode::k_maxChannels) );

    // swap buffers
    m_curBuf = 1 - m_curBuf;
}


}

