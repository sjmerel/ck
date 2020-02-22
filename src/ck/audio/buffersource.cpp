#include "ck/audio/buffersource.h"
#include "ck/audio/sample.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/logger.h"
#include <memory.h>

namespace Cki
{


BufferSource::BufferSource(const Sample& sample) :
    m_sampleInfo(sample.info),
    m_stream((void*) sample.data, sample.dataSize, sample.dataSize),
    m_loopCount((int) sample.info.loopCount),
    m_loopCurrent(0),
    m_loopStart((int) sample.info.loopStart),
    m_loopEnd((int) sample.info.loopEnd),
    m_releaseLoop(false)
{
    CK_ASSERT(sample.data);
    CK_ASSERT(sample.dataSize == sample.info.blocks * sample.info.blockBytes);
}

int BufferSource::read(void* buf, int blocks)
{
    int bytes = blocks * m_sampleInfo.blockBytes;
    if (!m_releaseLoop && (m_loopCount < 0 || m_loopCurrent < m_loopCount))
    {
        int endBlock = m_stream.getSize() / m_sampleInfo.blockBytes;
        int loopStartBlock = Math::clamp(Math::round((float) m_loopStart / m_sampleInfo.blockFrames), 0, endBlock-1);
        int loopEndBlock = m_loopEnd < 0 ? endBlock : Math::clamp(Math::round((float) m_loopEnd / m_sampleInfo.blockFrames), 1, endBlock);
        if (loopEndBlock <= loopStartBlock)
        {
            loopEndBlock = loopStartBlock + 1;
        }

        int bytesRead = 0;
        while (bytesRead < bytes)
        {
            int bytesToRead = Math::min(bytes - bytesRead, loopEndBlock * m_sampleInfo.blockBytes - m_stream.getPos());
            bytesRead += m_stream.read((byte*) buf + bytesRead, bytesToRead);
            if (m_stream.getPos() >= loopEndBlock * m_sampleInfo.blockBytes)
            {
                m_stream.setPos(loopStartBlock * m_sampleInfo.blockBytes);
                ++m_loopCurrent;
            }
            if (m_loopCount >= 0 && m_loopCurrent >= m_loopCount)
            {
                break;
            }
        }
        return bytesRead / m_sampleInfo.blockBytes;
    }
    else
    {
        return m_stream.read(buf, bytes) / m_sampleInfo.blockBytes;
    }
}

int BufferSource::getNumBlocks() const
{
    CK_ASSERT(m_stream.getSize() % m_sampleInfo.blockBytes == 0);
    return m_stream.getSize() / m_sampleInfo.blockBytes;
}

void BufferSource::setBlockPos(int block)
{
    m_stream.setPos(block * m_sampleInfo.blockBytes);
}

int BufferSource::getBlockPos() const
{
    CK_ASSERT(m_stream.getPos() % m_sampleInfo.blockBytes == 0);
    return m_stream.getPos() / m_sampleInfo.blockBytes;
}

void BufferSource::reset()
{
    m_stream.setPos(0);
    m_loopCurrent = 0;
    m_releaseLoop = false;
}

bool BufferSource::isLoopReleased() const
{
    return m_releaseLoop;
}

const SampleInfo& BufferSource::getSampleInfo() const
{
    return m_sampleInfo;
}

bool BufferSource::isInited() const
{
    return true;
}

bool BufferSource::isReady() const
{
    return true;
}

bool BufferSource::isFailed() const
{
    return false;
}

bool BufferSource::isDone() const
{
    bool looping = !m_releaseLoop && (m_loopCount < 0 || m_loopCurrent < m_loopCount);
    return !looping && m_stream.isAtEnd();
}

void BufferSource::setLoop(int loopStart, int loopEnd)
{
    m_loopStart = loopStart;
    m_loopEnd = loopEnd;
}

void BufferSource::getLoop(int& loopStart, int& loopEnd) const
{
    loopStart = m_loopStart;
    loopEnd = m_loopEnd;
}

void BufferSource::setLoopCount(int loopCount)
{
    m_loopCount = loopCount;
}

int BufferSource::getLoopCount() const
{
    return m_loopCount;
}

int BufferSource::getCurrentLoop() const
{
    return m_loopCurrent;
}

void BufferSource::releaseLoop()
{
    m_releaseLoop = true;
}

}
