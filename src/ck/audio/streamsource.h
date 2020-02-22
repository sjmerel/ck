#pragma once

#include "ck/core/platform.h"
#include "ck/core/ringbuffer.h"
#include "ck/core/mutex.h"
#include "ck/core/list.h"
#include "ck/audio/audiosource.h"

namespace Cki
{

class AudioStream;

class StreamSource : public AudioSource, public List<StreamSource>::Node
{
public:
    StreamSource(AudioStream*);
    virtual ~StreamSource();

    virtual int read(void* buf, int blocks);
    virtual int getNumBlocks() const;
    virtual void setBlockPos(int block);
    virtual int getBlockPos() const;

    virtual void reset();

    virtual const SampleInfo& getSampleInfo() const;

    virtual bool isInited() const;
    virtual bool isReady() const;
    virtual bool isFailed() const;
    virtual bool isDone() const;

    virtual void setLoop(int startFrame, int endFrame);
    virtual void getLoop(int& startFrame, int& endFrame) const;
    virtual void setLoopCount(int);
    virtual int getLoopCount() const;
    virtual int getCurrentLoop() const;
    virtual void releaseLoop();
    virtual bool isLoopReleased() const;

    // update from main thread
    virtual void update();

    // update from file thread
    void fileUpdate();
    static void fileUpdateAll();

private:
    AudioStream* m_stream;
    bool m_inited;
    bool m_failed;
    bool m_primed;
    bool m_reset;
    bool m_warned;
    int m_playBlock; // playback position (different from stream pos because of buffering)
    int m_nextBlock; // next block to jump to, or -1
    int m_loopCount;
    bool m_loopCountSet;
    int m_readLoopCurrent;
    int m_playLoopCurrent;
    bool m_readDone;
    bool m_playDone;
    int m_loopStart;
    int m_loopEnd;
    bool m_loopSet;
    bool m_releaseLoop;
    RingBuffer m_buf;
    Mutex m_bufMutex;

    static List<StreamSource> s_list;
    static Mutex s_listMutex;

    void fillBuffer();
    int readFromStream(int bytes);

    StreamSource(const StreamSource&);
    StreamSource& operator=(const StreamSource&);
};


}
