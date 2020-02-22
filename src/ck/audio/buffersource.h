#pragma once

#include "ck/core/platform.h"
#include "ck/core/bufferstream.h"
#include "ck/audio/audiosource.h"
#include "ck/audio/sampleinfo.h"

namespace Cki
{

struct Sample;

class BufferSource : public AudioSource
{
public:
    BufferSource(const Sample&);

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

private:
    const SampleInfo& m_sampleInfo;
    BufferStream m_stream;
    int m_loopCount; // total number of repeats; 0 = play once, -1 = play infinite
    int m_loopCurrent; // number of repeats 
    int m_loopStart;
    int m_loopEnd;
    bool m_releaseLoop;
};


}
