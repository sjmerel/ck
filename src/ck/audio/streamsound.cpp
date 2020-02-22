#include "ck/audio/streamsound.h"
#include "ck/audio/streamsource.h"
#include "ck/audio/audiograph.h"
#include "ck/core/path.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"

namespace Cki
{


StreamSound::StreamSound() :
    m_nextFrame(-1),
    m_nextMs(-1.0f)
{
}

bool StreamSound::isReadySub() const
{
    return m_nextFrame < 0 && m_nextMs < 0.0f && GraphSound::isReadySub();
}

void StreamSound::setPlayPosition(int frame)
{
    // unlike with BufferSource, StreamSource protects this call with a mutex,
    // so it's safe to call it from the main thread.  We want to do it
    // this way, rather than from SourceNode, so the seek can happen
    // even before the sound is played.
    m_nextFrame = frame;
    m_nextMs = -1.0f;
    updateSeek();
}

void StreamSound::setPlayPositionMs(float ms)
{
    m_nextMs = ms;
    m_nextFrame = -1;
    updateSeek();
}

void StreamSound::setLoop(int startFrame, int endFrame)
{
    if (m_playing)
    {
        CK_LOG_ERROR("Can't set loop points on a playing stream");
        return;
    }
    else
    {
        GraphSound::setLoop(startFrame, endFrame);
    }
}

void StreamSound::updateSeek()
{
    int nextFrame = -1;
    if (m_inited)
    {
        if (m_nextFrame >= 0)
        {
            nextFrame = m_nextFrame;
        }
        else if (m_nextMs >= 0.0f)
        {
            nextFrame = Math::round(m_nextMs * 0.001f * m_source->getSampleInfo().sampleRate);
        }
    }
    if (nextFrame >= 0)
    {
        AudioGraph::get()->execute(&m_sourceNode, SourceNode::k_setNextFrame, nextFrame);
        m_nextFrame = -1;
        m_nextMs = -1.0f;
    }
}


}
