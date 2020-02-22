#include "ck/audio/customaudiostream.h"
#include "ck/audio/audionode.h"
#include "ck/audio/audioformat.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"
#include "ck/customstream.h"

namespace Cki
{


CustomAudioStream::CustomAudioStream(CkCustomStream* stream) :
    m_stream(stream),
    m_inited(false),
    m_failed(false)
{
}

CustomAudioStream::~CustomAudioStream()
{
    delete m_stream;
}

void CustomAudioStream::init()
{
    if (!isFailed() && !m_inited)
    {
        m_stream->init();
        if (!m_stream->isFailed())
        {
            int channels = m_stream->getChannels();
            if (channels <= 0 || channels > 2)
            {
                CK_LOG_ERROR("Custom stream has %d channels; streams must have 1 or 2.", channels);
                m_failed = true;
                return;
            }

            int sampleRate = m_stream->getSampleRate();
            if (sampleRate <= 0 || sampleRate > AudioNode::k_maxSampleRate)
            {
                CK_LOG_ERROR("Custom stream has illegal sample rate (%d); must be between 1 and %d", sampleRate, AudioNode::k_maxSampleRate);
                m_failed = true;
                return;
            }

            m_sampleInfo.format = AudioFormat::k_pcmI16;
            m_sampleInfo.channels = (uint8) channels;
            m_sampleInfo.sampleRate = (uint16) sampleRate;
            m_sampleInfo.blocks = (uint32) m_stream->getLength();
            m_sampleInfo.blockBytes = (uint16) (channels * sizeof(int16));
            m_sampleInfo.blockFrames = 1;

            m_inited = true;
        }
    }
}

bool CustomAudioStream::isFailed() const
{
    return m_failed || m_stream->isFailed();
}

int CustomAudioStream::read(void* buf, int blocks)
{
    CK_ASSERT(m_inited && !isFailed());
    return m_stream->read((int16*) buf, blocks);
}

int CustomAudioStream::getNumBlocks() const
{
    CK_ASSERT(m_inited && !isFailed());
    return m_stream->getLength();
}

void CustomAudioStream::setBlockPos(int block)
{
    CK_ASSERT(m_inited && !isFailed());
    return m_stream->setPlayPosition(block);
}

int CustomAudioStream::getBlockPos() const
{
    CK_ASSERT(m_inited && !isFailed());
    return m_stream->getPlayPosition();
}



}
