#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/audio/sampleinfo.h"

namespace Cki
{


class AudioSource
{
public:
    virtual ~AudioSource() {};

    virtual int read(void* buf, int blocks) = 0;
    virtual int getNumBlocks() const = 0;
    virtual void setBlockPos(int block) = 0;
    virtual int getBlockPos() const = 0;

    virtual void reset() = 0;

    // this can be called after inited
    virtual const SampleInfo& getSampleInfo() const = 0;

    virtual bool isInited() const = 0; // has read format info
    virtual bool isReady() const = 0; // ready to play
    virtual bool isFailed() const = 0;
    virtual bool isDone() const = 0;

    virtual void setLoop(int startFrame, int endFrame) = 0;
    virtual void getLoop(int& startFrame, int& endFrame) const = 0;
    virtual void setLoopCount(int) = 0;
    virtual int getLoopCount() const = 0;
    virtual int getCurrentLoop() const = 0;
    virtual void releaseLoop() = 0;
    virtual bool isLoopReleased() const = 0;

    virtual void update() {}
};


}
