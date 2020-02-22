#include "ck/audio/graphoutputjava_android.h"
#include "ck/audio/audiotrackproxy_android.h"
#include "ck/audio/audioutil.h"
#include "ck/core/debug.h"
#include "ck/core/mem.h"
#include "ck/core/system.h"
#include "ck/core/jnienv_android.h"

namespace Cki
{

GraphOutputJavaAndroid::GraphOutputJavaAndroid() :
    m_audioTrack(NULL),
    m_data(NULL),
    m_dataFrames(0),
    m_bufFrames(0),
    m_writeCount(0),
    m_readPos(0),
    m_readCount(0),
    m_updateThread(updateThreadFunc),
    m_updateThreadDone(false)
{
    m_updateThread.setPriority(Thread::k_priorityMax);
    m_updateThread.setFlags(Thread::k_flagAttachToJvm);
    m_updateThread.setName("CK java audio output");

    m_bufferMs = System::get()->getConfig().audioUpdateMs;
    m_dataFrames = (int) (m_bufferMs * AudioNode::getSampleRate() * 0.001f);
    int minBufSize = AudioTrackProxy::getMinBufferSize(AudioNode::getSampleRate(), AudioNode::k_maxChannels);
    int minBufFrames = minBufSize / (sizeof(int16) * AudioNode::k_maxChannels);

    // make buffer a multiple of the slice size
    if (minBufFrames % m_dataFrames)
    {
        minBufFrames += m_dataFrames - (minBufFrames % m_dataFrames);
    }

    // make buffer a multiple of the slice size
    m_bufFrames = minBufFrames;

    JniEnv jni;
    jshortArray array = jni->NewShortArray(m_dataFrames * AudioNode::k_maxChannels);
    m_data = (jshortArray) jni->NewGlobalRef(array);

    m_renderBuf = (int32*) Mem::alloc(m_dataFrames * sizeof(int32) * AudioNode::k_maxChannels);
}

GraphOutputJavaAndroid::~GraphOutputJavaAndroid()
{
    stop();
    delete m_audioTrack;

    JniEnv jni;
    jni->DeleteGlobalRef(m_data);
    
    Mem::free(m_renderBuf);
}

void GraphOutputJavaAndroid::startImpl()
{
    m_audioTrack = new AudioTrackProxy(AudioNode::getSampleRate(), AudioNode::k_maxChannels, m_bufFrames);

    // have to fill the buffer with 0s to start it playing
    initBuffer();

    m_writeCount = m_bufFrames;
    m_readPos = 0;
    m_readCount = 0;

    m_updateThreadDone = false;
    m_updateThread.start(this);

    m_audioTrack->play();
}

void GraphOutputJavaAndroid::stopImpl()
{
    m_updateThreadDone = true;
    m_updateThread.join();

    m_audioTrack->stop();
    m_audioTrack->flush();

    // XXX audiotrack class is very buggy; sometimes fails to play after
    // stopping and playing again.  best to just create a new one each time!
    delete m_audioTrack;
    m_audioTrack = NULL;
}

////////////////////////////////////////

void* GraphOutputJavaAndroid::updateThreadFunc(void* arg)
{
    GraphOutputJavaAndroid* me = (GraphOutputJavaAndroid*) arg;
    me->updateLoop();
    return NULL;
}

void GraphOutputJavaAndroid::updateLoop()
{
    while (!m_updateThreadDone)
    {
        int pos = m_audioTrack->getPlaybackHeadPosition();

        if (pos > m_readPos)
        {
            m_readCount += (pos - m_readPos);
        }
        else
        {
            m_readCount += m_bufFrames - (m_readPos - pos);
        }
        m_readPos = pos;

        const int k_sliceLag = 1;
        while (m_readCount >= m_writeCount - k_sliceLag*m_dataFrames)
        {
            renderBuffer();
        }

        // prevent overflow
        const int k_countMax = 0x40000000;
        if (m_readCount > k_countMax && m_writeCount > k_countMax)
        {
            m_readCount -= k_countMax;
            m_writeCount -= k_countMax;
        }

        Thread::sleepMs(m_bufferMs * 0.5f);
    }
}

void GraphOutputJavaAndroid::initBuffer()
{
    JniEnv jni;

    jboolean isCopy;
    jshort* buf = jni->GetShortArrayElements(m_data, &isCopy);

    Mem::clear(buf, m_dataFrames * AudioNode::k_maxChannels * sizeof(int16));

    // TODO: use JNI_COMMIT after the first call?
    jni->ReleaseShortArrayElements(m_data, buf, 0);

    int numSlices = m_bufFrames / m_dataFrames;
    for (int i = 0; i < numSlices; ++i)
    {
        int written = m_audioTrack->write(m_data, m_dataFrames * AudioNode::k_maxChannels);
        CK_UNUSED(written);
        CK_ASSERT(written == m_dataFrames * AudioNode::k_maxChannels);
    }
}

void GraphOutputJavaAndroid::renderBuffer()
{
    JniEnv jni;

    jboolean isCopy;
    jshort* buf = jni->GetShortArrayElements(m_data, &isCopy);

    if (System::get()->getSampleType() == kCkSampleType_Float)
    {
        render((float*) m_renderBuf, m_dataFrames);
        AudioUtil::convert((float*) m_renderBuf, buf, m_dataFrames*2);
    }
    else
    {
        render((int32*) m_renderBuf, m_dataFrames);
        AudioUtil::convert((int32*) m_renderBuf, buf, m_dataFrames*2);
    }

    // TODO: use JNI_COMMIT after the first call?
    jni->ReleaseShortArrayElements(m_data, buf, 0);

    int written = m_audioTrack->write(m_data, m_dataFrames * AudioNode::k_maxChannels);
    CK_UNUSED(written);
    CK_ASSERT(written == m_dataFrames * AudioNode::k_maxChannels);

    m_writeCount += m_dataFrames;
}


}


