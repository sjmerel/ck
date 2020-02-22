#include "ck/audio/mixnode.h"
#include "ck/audio/audioutil.h"
#include "ck/audio/audiograph.h"
#include "ck/core/mem.h"
#include "ck/core/logger.h"
#include "ck/core/math.h"
#include "ck/core/debug.h"
#include "ck/core/system.h"

namespace Cki
{


MixNode::MixNode() :
    m_inputs(),
    m_bufMem(NULL)
{
    allocateBuffer(); // don't add to pool on main thread
}

MixNode::~MixNode()
{
    Mem::free(m_bufMem);
}

BufferHandle MixNode::processFloat(int frames, bool post, bool& needsPost)
{
    return process<float>(frames, post, needsPost);
}

BufferHandle MixNode::processFixed(int frames, bool post, bool& needsPost)
{
    return process<int32>(frames, post, needsPost);
}

template <typename T>
BufferHandle MixNode::process(int frames, bool post, bool& needsPost)
{
    // find first active input
    BufferHandle firstBufHandle;
    AudioNode* input = m_inputs.getFirst();
    while (input)
    {
        firstBufHandle = input->process<T>(frames, post, needsPost);
        input = input->getNext();

        if (firstBufHandle.isValid())
        {
            break;
        }
    }

    if (!firstBufHandle.isValid())
    {
        return firstBufHandle; // no active inputs
    }

    if (!m_buf.getMem())
    {
        if (!m_bufMem)
        {
            allocateBuffer(); // since alloc may have failed first time
        }

        if (m_bufMem)
        {
            m_buf.init(m_bufMem);
            getBufferPool()->add(m_buf);
        }
        else
        {
            return firstBufHandle;
        }
    }

    AudioGraph* graph = AudioGraph::get();
    const CkConfig& config = System::get()->getConfig();
    float renderLimitMs = config.audioUpdateMs * config.maxRenderLoad;
#if CK_PLATFORM_WP8
    // seem to get "false positives" on wp8; i.e. warning is shown with no audible artifacts.  maybe some internal buffering in xaudio2?
    renderLimitMs += 2.0f;
#endif

    // bail out if rendering is taking too long
    if (graph->getRenderMs() > renderLimitMs)
    {
        printCpuWarning(graph->getRenderMs());
        return firstBufHandle;
    }

    while (input)
    {
        BufferHandle nextBufHandle = input->process<T>(frames, post, needsPost);
        if (nextBufHandle.isValid())
        {
            // bail out if rendering is taking too long
            if (graph->getRenderMs() > renderLimitMs)
            {
                printCpuWarning(graph->getRenderMs());
                break;
            }

            AudioUtil::mix((const T*) nextBufHandle.get(), (T*) firstBufHandle.get(), frames * k_maxChannels);
        }
        input = input->getNext();
    }

    return firstBufHandle;
}

void MixNode::execute(int cmdId, CommandParam param0, CommandParam param1)
{
    switch (cmdId)
    {
        case k_addInput:
            addInput((AudioNode*) param0.addrValue);
            break;

        case k_removeInput:
            removeInput((AudioNode*) param0.addrValue);
            break;

        case k_detach:
            {
                BufferPool* pool = getBufferPool();
                if (pool)
                {
                    pool->remove(m_buf);
                }
            }
            break;

        default:
            CK_FAIL("unknown command");
    };
}

void MixNode::addInput(AudioNode* src)
{
    m_inputs.addFirst(src);
}

void MixNode::removeInput(AudioNode* src)
{
    m_inputs.remove(src);
}

#if CK_DEBUG
void MixNode::print(int level)
{
    printImpl(level, "mix");
    AudioNode* input = m_inputs.getFirst();
    while (input)
    {
        input->print(level+1);
        input = input->getNext();
    }
}
#endif

void MixNode::allocateBuffer()
{
    // 64-byte alignment (16 words) for faster ARM NEON accesses
    m_bufMem = Mem::alloc(getBufferPool()->getBufferSize() * sizeof(int32), 64);
    if (!m_bufMem)
    {
        CK_LOG_ERROR("Could not allocate mix buffer");
    }
}

Timer MixNode::s_cpuWarningTimer;

void MixNode::printCpuWarning(float ms)
{
    if (!s_cpuWarningTimer.isRunning() || s_cpuWarningTimer.getElapsedMs() > 500.0f)
    {
        s_cpuWarningTimer.start();
        s_cpuWarningTimer.reset();
        CK_LOG_WARNING("Audio rendering is taking too long (%f ms); some sounds will not be played.", ms);
    }
}


}
