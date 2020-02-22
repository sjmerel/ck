#include "ck/audio/graphoutput_dummy.h"
#include "ck/core/debug.h"
#include "ck/core/thread.h"
#include "ck/core/mem.h"

namespace
{
    const int k_frames = 256;
}

namespace Cki
{

GraphOutputDummy* GraphOutputDummy::s_inst = NULL;

GraphOutputDummy::GraphOutputDummy(bool polled) :
    m_thread(NULL),
    m_stopThread(false)
{
    m_buf = Mem::alloc(k_frames * AudioNode::k_maxChannels * sizeof(int32));
    CK_ASSERT(!s_inst);
    s_inst = this;

    if (!polled)
    {
        m_thread = new Thread(threadFunc);
        m_thread->start(this);
    }
}

GraphOutputDummy::~GraphOutputDummy()
{
    stopImpl();

    if (m_thread)
    {
        m_stopThread = true;
        m_thread->join();
        delete m_thread;
    }
    Mem::free(m_buf);
    s_inst = NULL;
}

void GraphOutputDummy::startImpl()
{
}

void GraphOutputDummy::stopImpl()
{
}

////////////////////////////////////////

void* GraphOutputDummy::threadFunc(void* arg)
{
    s_inst->threadLoop();
    return 0;
}

void GraphOutputDummy::threadLoop()
{
    while (!m_stopThread)
    {
        doRender();
        Thread::sleepMs(100); // TODO use CkConfig.audioUpdateMs?
    }
}

void GraphOutputDummy::doRender()
{
    if (isRunning())
    {
        if (m_fixedPoint)
        {
            render((int32*) m_buf, k_frames);
        }
        else
        {
            render((float*) m_buf, k_frames);
        }
    }
}

void GraphOutputDummy::poll()
{
    CK_ASSERT(!s_inst->m_thread);
    s_inst->doRender();
}


}



