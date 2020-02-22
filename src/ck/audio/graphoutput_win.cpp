#include "ck/audio/graphoutput_win.h"
#include "ck/audio/audio.h"
#include "ck/audio/audio_win.h"
#include "ck/audio/audiodebug_win.h"
#include "ck/audio/audioutil.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/mem.h"
#include "ck/core/system.h"

namespace Cki
{


GraphOutputWin::GraphOutputWin() :
    m_bufMem(0),
    m_bufSize(0),
    m_curBuf(0)
{
    Audio::createVoice(bufferEnd, this);
    float bufferMs = System::get()->getConfig().audioUpdateMs;
    int bufFrames = Math::round(bufferMs * 0.001f * AudioNode::k_maxSampleRate);
    int bufBytes = bufFrames * AudioNode::k_maxChannels * sizeof(float);

    // 16-byte alignment for SSE
    if (bufBytes % 16 != 0)
    {
        bufBytes += (16 - bufBytes % 16);
    }

    m_bufMem = (float*) Mem::alloc(bufBytes*2, 16); // 16-byte alignment for SSE
    m_bufs[0] = m_bufMem;
    m_bufs[1] = m_bufMem + bufBytes/sizeof(float);
    m_bufSize = bufBytes;
}

GraphOutputWin::~GraphOutputWin()
{
    stopImpl();
    Audio::destroyVoice();
    Mem::free(m_bufMem);
}

void GraphOutputWin::startImpl()
{
    // prime with empty buffer (shouldn't render on main thread)
    submitBuffer(false);

    Audio::startVoice();
}

void GraphOutputWin::stopImpl()
{
    Audio::stopVoice();
}

////////////////////////////////////////

void GraphOutputWin::submitBuffer(bool rendered)
{
    float* buf = m_bufs[m_curBuf];
    if (rendered)
    {
        if (System::get()->getSampleType() == kCkSampleType_Float)
        {
            render((float*) buf, m_bufSize / (AudioNode::k_maxChannels * sizeof(float)));
        }
        else
        {
            render((int32*) buf, m_bufSize / (AudioNode::k_maxChannels * sizeof(int32)));
            AudioUtil::convert((const int32*) buf, (float*) m_bufs[m_curBuf], m_bufSize / sizeof(int32));
        }
    }
    else
    {
        Mem::clear(buf, m_bufSize);
    }

    Audio::submitVoiceBuffer(buf, m_bufSize);
    m_curBuf = 1 - m_curBuf;
}

void GraphOutputWin::bufferEnd(void* data)
{
    GraphOutputWin* me = (GraphOutputWin*) data;
    if (me->isRunning())
    {
        me->submitBuffer(true);
    }
}


}


