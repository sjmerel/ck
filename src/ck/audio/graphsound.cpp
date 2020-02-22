#include "ck/audio/graphsound.h"
#include "ck/audio/audiograph.h"
#include "ck/audio/audiosource.h"
#include "ck/audio/sourcenode.h"
#include "ck/audio/mixnode.h"
#include "ck/core/logger.h"
#include "ck/core/math.h"

// template definitions
#include "ck/core/list.cpp"


namespace Cki
{


GraphSound::GraphSound() :
    m_source(NULL),
    m_decoder(),
    m_sourceNode(),
    m_outMixNode(NULL),
    m_fx(NULL),
    m_inited(false),
    m_playing(false),
    m_loopCount(0),
    m_loopStart(0),
    m_loopEnd(-1),
    m_playId(0)
{
}

void GraphSound::destroy()
{
    CK_ASSERT(m_source);

    destroyImpl();

    m_source = NULL;

    if (m_fx)
    {
        m_fx->removeInputSound(this);
    }

    AudioGraph::get()->deleteObject(this);
}

bool GraphSound::isFailed() const
{
    return m_source->isFailed();
}

bool GraphSound::isPlaying() const
{
    return (m_playing && m_sourceNode.getDoneId() != m_playId) || (m_sourceNode.getPlayId() == -1);
}

void GraphSound::setLoop(int startFrame, int endFrame)
{
    startFrame = Math::max(startFrame, 0); // endFrame can be <0
    if (startFrame >= endFrame && endFrame >= 0)
    {
        CK_LOG_ERROR("loop start frame must be less than end frame");
        return;
    }

    if (m_loopStart != startFrame || m_loopEnd != endFrame)
    {
        m_loopStart = startFrame;
        m_loopEnd = endFrame;
        AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_setLoop, startFrame, endFrame);
    }
}

void GraphSound::getLoop(int& startFrame, int& endFrame) const
{
    startFrame = m_loopStart;
    endFrame = m_loopEnd;
}

void GraphSound::setLoopCount(int loopCount)
{
    if (m_loopCount != loopCount)
    {
        m_loopCount = loopCount;
        AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_setLoopCount, loopCount);
    }
}

int GraphSound::getLoopCount() const
{
    return m_loopCount;
}

int GraphSound::getCurrentLoop() const
{
    return m_source->getCurrentLoop();
}

void GraphSound::releaseLoop()
{
    AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_releaseLoop, 0);
}

bool GraphSound::isLoopReleased() const
{
    return m_source->isLoopReleased();
}

int GraphSound::getPlayPosition() const
{
    if (m_inited)
    {
        return m_decoder.get()->getFramePos();
    }
    else
    {
        return 0;
    }
}

float GraphSound::getPlayPositionMs() const
{
    if (m_inited)
    {
        return 1000.0f * m_decoder.get()->getFramePos() / m_source->getSampleInfo().sampleRate;
    }
    else
    {
        return 0.0f; // don't know sample rate, but we must be at 0
    }
}

void GraphSound::setNextSound(CkSound* nextSound)
{
    Sound::setNextSound(nextSound);

    SourceNode* nextSource = NULL;
    if (nextSound)
    {
        nextSource = ((Sound*) nextSound)->getSourceNode();
    }
    AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_setNext, nextSource);
}

int GraphSound::getLength() const
{
    if (m_source->isInited())
    {
        const SampleInfo& info = m_source->getSampleInfo();
        return info.blocks * info.blockFrames;
    }
    else
    {
        return -1;
    }
}

float GraphSound::getLengthMs() const
{
    if (m_source->isInited())
    {
        const SampleInfo& info = m_source->getSampleInfo();
        int frames = info.blocks * info.blockFrames;
        return 1000.0f * frames / info.sampleRate;
    }
    else
    {
        return -1;
    }
}

int GraphSound::getSampleRate() const
{
    return m_source->isInited() ? m_source->getSampleInfo().sampleRate : -1;
}

int GraphSound::getChannels() const
{
    return m_source->isInited() ? m_source->getSampleInfo().channels : -1;
}

void GraphSound::setEffectBus(CkEffectBus* fx)
{
    if (m_fx)
    {
        m_fx->removeInputSound(this);
    }

    if (fx)
    {
        ((EffectBus*) fx)->addInputSound(this);
    }
    
    m_fx = (EffectBus*) fx;

    if (m_inited)
    {
        updateMixNode();
    }
}

EffectBus* GraphSound::getEffectBus()
{
    return m_fx;
}

void GraphSound::updateVolumeAndPan()
{
    if (m_inited)
    {
        AudioGraph* graph = AudioGraph::get();

        VolumeMatrix volMat;
        getFinalVolumeMatrix(volMat);

        graph->execute(&m_sourceNode, SourceNode::k_setVolumeL, volMat.ll, volMat.lr);
        graph->execute(&m_sourceNode, SourceNode::k_setVolumeR, volMat.rl, volMat.rr);
    }
}

SourceNode* GraphSound::getSourceNode()
{
    return &m_sourceNode;
}


////////////////////////////////////////

// subInit is are called in ctor of derived classes,
// since their members are not yet initialized in the GraphSound ctor

void GraphSound::subInit(AudioSource* source)
{
    CK_ASSERT(!m_source);
    m_source = source;

    m_sourceNode.init(source);
    Sound::update();
}

void GraphSound::playSub()
{
    if (!m_inited && m_source->isInited())
    {
        init();
    }

    ++m_playId;
    if (m_playId == CK_INT32_MAX)
    {
        m_playId = 1;
    }

    AudioGraph* graph = AudioGraph::get();
    graph->execute(&m_sourceNode, SourceNode::k_setPlaying, m_playId);

    if (!isVirtual())
    {
        graph->execute(&m_sourceNode, SourceNode::k_connect);
    }

    m_playing = true;
}

void GraphSound::updateSub()
{
    m_source->update();

    if (!m_inited && m_source->isInited())
    {
        init();
    }

    updateSeek(); // seek in stream

    if (m_playing && m_sourceNode.getDoneId() == m_playId)
    {
        m_playing = false;
    }
}

void GraphSound::stopSub()
{
    if (isPlaying())
    {
        AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_setPlaying, 0);
        m_playing = false;
    }
}

bool GraphSound::isReadySub() const
{
    return m_source->isReady();
}

void GraphSound::updatePaused()
{
    AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_setPaused, getMixedPauseState());
}

void GraphSound::updateSpeed()
{
    if (m_inited)
    {
        float speed = getFinalSpeed();
        AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_setSpeed, speed);
    }
}

void GraphSound::updateVirtual()
{
    if (m_playing)
    {
        if (isVirtual())
        {
            AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_disconnect);
        }
        else
        {
            AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_connect);
        }
    }
}


////////////////////////////////////////

void GraphSound::init()
{
    if (!m_inited)
    {
        // we now know sample rate and data format

        const SampleInfo& info = m_source->getSampleInfo();

        if (!m_volumeSet)
        {
            m_volume = (float) info.volume / CK_UINT16_MAX;
        }
        if (!m_panSet)
        {
            m_pan = (float) info.pan / CK_INT16_MAX;
        }

        m_source->getLoop(m_loopStart, m_loopEnd);
        m_loopCount = m_source->getLoopCount();

        updateMixNode();

        m_decoder.init(*m_source);
        AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_setDecoder, m_decoder.get());

        m_inited = true;
        updateVolumeAndPan();
    }
}

void GraphSound::updateMixNode()
{
    AudioGraph* graph = AudioGraph::get();

    MixNode* newMixNode = (m_fx ? m_fx->getInput() : graph->getOutputMixer());

    if (m_outMixNode != newMixNode)
    {
        graph->execute(&m_sourceNode, SourceNode::k_setOutput, newMixNode);
        m_outMixNode = newMixNode;
    }
}

}

