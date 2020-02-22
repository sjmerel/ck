#include "ck/audio/streamsource.h"
#include "ck/audio/audiostream.h"
#include "ck/audio/audioutil.h"
#include "ck/audio/fileheader.h"
#include "ck/audio/filetype.h"
#include "ck/core/swapstream.h"
#include "ck/core/system.h"
#include "ck/core/math.h"
#include "ck/core/debug.h"
#include "ck/core/mutexlock.h"
#include "ck/core/logger.h"
#include "ck/core/mem.h"

// template definitions
#include "ck/core/list.cpp"


namespace Cki
{



StreamSource::StreamSource(AudioStream* stream) :
    m_stream(stream),
    m_inited(false),
    m_failed(false),
    m_primed(false),
    m_reset(false),
    m_warned(false),
    m_playBlock(0),
    m_nextBlock(-1),
    m_loopCount(0),
    m_loopCountSet(false),
    m_readLoopCurrent(0),
    m_playLoopCurrent(0),
    m_readDone(false),
    m_playDone(false),
    m_loopStart(0),
    m_loopEnd(-1),
    m_loopSet(false),
    m_releaseLoop(false),
    m_buf()
{
    MutexLock lock(s_listMutex);
    s_list.addFirst(this);
}

StreamSource::~StreamSource()
{
    MutexLock lock(s_listMutex);
    s_list.remove(this);
}

int StreamSource::read(void* buf, int blocks)
{
    // lock because read is usually called from audio output callback
    MutexLock lock(m_bufMutex);

    if (m_playDone)
    {
        return 0;
    }

    CK_ASSERT(m_inited);
    int blockBytes = m_stream->getSampleInfo().blockBytes;
    int blockFrames = m_stream->getSampleInfo().blockFrames;

    int bytes = blocks * blockBytes;
    int bytesRead = m_buf.read(buf, bytes);
    CK_ASSERT(bytesRead % blockBytes == 0);
    int blocksRead = bytesRead / blockBytes;

    m_playBlock += blocksRead;

    // adjust for loops
    // TODO: if releaseLoop() is called, release now, rather than waiting
    // for file thread to finish
    // Note that getNumBlocks() can return -1, but will not if we've played to the end,
    // which will be the case if we're looping at the end.
    int numBlocks = m_stream->getNumBlocks();
    int loopEndBlock = m_loopEnd < 0 ? numBlocks : m_loopEnd / blockFrames;
    if (loopEndBlock > 0)
    {
        int loopStartBlock = Math::clamp(Math::round((float) m_loopStart / blockFrames), 0, loopEndBlock - 1);
        while ((m_loopCount < 0 || m_playLoopCurrent < m_loopCount) &&
                m_playBlock >= loopEndBlock)
        {
            m_playBlock -= (loopEndBlock - loopStartBlock);
            ++m_playLoopCurrent;
        }
    }

    bool underrun = false;
    if (blocksRead < blocks)
    {
        if (m_readDone)
        {
            m_playDone = true;
            m_playBlock = 0;
        }
        else
        {
            underrun = true;
            if (!m_warned)
            {
                CK_LOG_WARNING("Stream buffer underrun! Try increasing CkConfig.streamBufferMs.");
                m_warned = true;
            }
        }
    }

    if (!underrun)
    {
        m_warned = false;
    }

    return blocksRead;
}

int StreamSource::getNumBlocks() const
{
    return m_stream->getNumBlocks();
}

void StreamSource::setBlockPos(int block)
{
    MutexLock lock(m_bufMutex);
    int pos = getBlockPos();
    if (block != m_nextBlock && block != pos)
    {
        // if the buffer is primed, and we're seeking forward to within the buffer, don't reset it
        if (m_primed && !m_reset && block > pos)
        {
            const SampleInfo& sampleInfo = m_stream->getSampleInfo();
            int bufBlocks = m_buf.getStoredBytes() / sampleInfo.blockBytes;
            if (bufBlocks > block - pos)
            {
                m_buf.consume((block - pos) * sampleInfo.blockBytes);
                if (m_buf.getStoredBytes() < m_buf.getSize() / 2)
                {
                    m_primed = false;
                }
                return;
            }
        }

        // otherwise reset and fill the buffer from scratch
        m_reset = true;
        m_primed = false;
        m_nextBlock = block;
    }
}

int StreamSource::getBlockPos() const
{
    return m_playBlock;
}

void StreamSource::reset()
{
    if (m_inited)
    {
        MutexLock lock(m_bufMutex);
        m_reset = true;
        m_playBlock = 0;
        m_nextBlock = 0;
        m_readLoopCurrent = 0;
        m_playLoopCurrent = 0;
        m_readDone = false;
        m_playDone = false;
        m_primed = false;
        m_releaseLoop = false;
    }
}

const SampleInfo& StreamSource::getSampleInfo() const
{
    CK_ASSERT(m_inited);
    return m_stream->getSampleInfo();
}

bool StreamSource::isInited() const
{
    return m_inited && !m_failed && !m_stream->isFailed();
}

bool StreamSource::isReady() const
{
    return m_inited && !m_failed && !m_stream->isFailed() && m_primed && !m_reset;
}

bool StreamSource::isFailed() const
{
    return m_failed || m_stream->isFailed();
}

bool StreamSource::isDone() const
{
    return m_playDone;
}

void StreamSource::setLoop(int loopStart, int loopEnd)
{
    MutexLock lock(m_bufMutex); 
    m_loopSet = true;
    m_loopStart = loopStart;
    m_loopEnd = loopEnd;
}

void StreamSource::getLoop(int& loopStart, int& loopEnd) const
{
    loopStart = m_loopStart;
    loopEnd = m_loopEnd;
}

void StreamSource::setLoopCount(int loopCount)
{
    MutexLock lock(m_bufMutex); // probably not necessary
    m_loopCountSet = true;
    m_loopCount = loopCount;
}

int StreamSource::getLoopCount() const
{
    return m_loopCount;
}

int StreamSource::getCurrentLoop() const
{
    return m_playLoopCurrent;
}

void StreamSource::releaseLoop()
{
    MutexLock lock(m_bufMutex); 
    m_releaseLoop = true;
}

bool StreamSource::isLoopReleased() const
{
    return m_releaseLoop;
}

void StreamSource::update()
{
    if (m_inited && !m_failed && !m_stream->isFailed() && !m_buf.isInited())
    {
        // allocate buffer
        const SampleInfo& sampleInfo = m_stream->getSampleInfo();
        int bytesPerSec = sampleInfo.sampleRate * sampleInfo.blockBytes / sampleInfo.blockFrames;
        float bufferMs = System::get()->getConfig().streamBufferMs;
        int bufBytes = Math::round(bufferMs * 0.001f * bytesPerSec);
        bufBytes -= (bufBytes % sampleInfo.blockBytes); // make multiple of block size
        m_buf.init(bufBytes);
        if (m_buf.isInited())
        {
            // set default loops (only if not set earlier explicitly)
            if (!m_loopSet)
            {
                m_loopStart = sampleInfo.loopStart;
                m_loopEnd = sampleInfo.loopEnd;
            }
            if (!m_loopCountSet)
            {
                m_loopCount = sampleInfo.loopCount;
            }
        }
        else
        {
            m_failed = true;
        }
    }
}

void StreamSource::fileUpdate()
{
    if (isFailed())
    {
        return;
    }

    MutexLock lock(m_bufMutex);

    if (!m_inited)
    {
        m_stream->init();
        if (m_stream->isFailed())
        {
            return;
        }
        else
        {
            m_inited = true;
        }

        // buffer will be allocated in next update() call (on main thread)
    }

    if (m_reset)
    {
        m_buf.resetWrite();
        m_reset = false;
    }

    if (!m_readDone && m_buf.isInited() && m_buf.getStoredBytes() < m_buf.getSize() / 2)
    {
        fillBuffer();
        m_primed = true;
    }
}

void StreamSource::fileUpdateAll()
{
    MutexLock lock(s_listMutex);
    StreamSource* p = s_list.getFirst();
    while (p)
    {
        p->fileUpdate();
        p = p->getNext();
    }
}

////////////////////////////////////////

void StreamSource::fillBuffer()
{
    int freeBytes = m_buf.getFreeBytes();
    CK_ASSERT(m_inited);
    const SampleInfo& sampleInfo = m_stream->getSampleInfo();

    if (!m_releaseLoop && (m_loopCount < 0 || m_readLoopCurrent < m_loopCount))
    {
        // loop end may be -1 to indicate end of file; note that WE DO NOT KNOW THE FILE LENGTH FOR CERTAIN!
        int loopStartBlock = Math::round((float) m_loopStart / sampleInfo.blockFrames);
        int loopEndBlock = m_loopEnd < 0 ? -1 : Math::round((float) m_loopEnd / sampleInfo.blockFrames);
        if (loopEndBlock >= 0 && loopEndBlock <= loopStartBlock)
        {
            loopEndBlock = loopStartBlock + 1;
        }

        int totalBytesRead = 0;
        while (totalBytesRead < freeBytes)
        {
            int bytesToRead = freeBytes - totalBytesRead;
            if (loopEndBlock >= 0)
            {
                bytesToRead = Math::min(bytesToRead, (loopEndBlock - m_stream->getBlockPos()) * sampleInfo.blockBytes);
            }

            int bytesRead = readFromStream(bytesToRead);
            totalBytesRead += bytesRead;

            if ((loopEndBlock >= 0 && m_stream->getBlockPos() >= loopEndBlock) || bytesRead < bytesToRead)
            {
                // hit loop end block, or the end of the file
                loopStartBlock = Math::min(loopStartBlock, m_stream->getBlockPos() - 1);
                m_stream->setBlockPos(loopStartBlock);
                ++m_readLoopCurrent;
            }
            if (m_loopCount >= 0)
            {
                if (m_readLoopCurrent > m_loopCount && bytesRead < bytesToRead)
                {
                    m_readDone = true;
                }
                if (m_readLoopCurrent >= m_loopCount)
                {
                    break;
                }
            }
        }
    }
    else
    {
        int bytesRead = readFromStream(freeBytes);
        if (bytesRead < freeBytes)
        {
            m_readDone = true;
        }
    }
}

int StreamSource::readFromStream(int bytes)
{
    if (m_nextBlock >= 0)
    {
        m_stream->setBlockPos(m_nextBlock);
        m_nextBlock = -1;
        m_playBlock = m_stream->getBlockPos();
    }

    // Some streams do not reliably report their length (I've noticed
    // this with MP3 files, specifically 0001.mp3 in the testbed).
    // So try to read an entire buffer's worth; when we hit the
    // actual end of the stream, we will read less than requested.
    int blockBytes = m_stream->getSampleInfo().blockBytes;
    int bytesToWrite = Math::min(bytes, m_buf.getSize());

    if (bytesToWrite > 0)
    {
        void* buf1;
        void* buf2;
        int bytes1;
        int bytes2;

        m_buf.beginWrite(bytesToWrite, buf1, bytes1, buf2, bytes2);
        CK_ASSERT(bytes1 % blockBytes == 0);
        CK_ASSERT(bytes2 % blockBytes == 0);

        // unlock mutex during read
        m_bufMutex.unlock();
        int blocksRead = 0;
        if (buf1)
        {
            blocksRead += m_stream->read(buf1, bytes1 / blockBytes);
            if (buf2)
            {
                blocksRead += m_stream->read(buf2, bytes2 / blockBytes);
            }
        }
        m_bufMutex.lock();

        int bytesRead = blocksRead * blockBytes;
        m_buf.endWrite(bytesRead);
        return bytesRead;
    }
    else
    {
        return 0;
    }
}


List<StreamSource> StreamSource::s_list;
Mutex StreamSource::s_listMutex;

template class List<StreamSource>;

}
