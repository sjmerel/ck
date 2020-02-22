#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/core/list.h"
#include "ck/audio/bufferpool.h"
#include "ck/audio/bufferhandle.h"
#include "ck/audio/commandparam.h"
#include "ck/audio/volumematrix.h"
#include "ck/audio/commandobject.h"

namespace Cki
{


class AudioNode : 
    public Allocatable, 
    public List<AudioNode>::Node, 
    public CommandObject
{
public:
    enum { k_maxChannels = 2 };
    enum { k_maxSampleRate = 48000 };

    static void init();
    static void shutdown();

    AudioNode();
    virtual ~AudioNode();

    virtual BufferHandle processFloat(int frames, bool post, bool& needsPost) = 0;
    virtual BufferHandle processFixed(int frames, bool post, bool& needsPost) = 0;
    template <typename T> BufferHandle process(int frames, bool post, bool& needsPost);

#if CK_DEBUG
    virtual void print(int level) = 0;
    static void verify() { s_pool->verify(); }
#endif

    static BufferHandle acquireBuffer();
    static int getBufferSamples();
    static int getBufferFrames();
    static int getSampleRate();

    static void setVolumeRampTime(float); // ms per volume unit
    static float getVolumeRampTime();

protected:
    static float s_volRampTime;

    float getVolumeRate(); // volume units per sample
    static BufferPool* getBufferPool();

#if CK_DEBUG
    void printImpl(int level, const char* name);
#endif

private:
    static BufferPool* s_pool;
    static int s_sampleRate;
};

////////////////////////////////////////

inline
int AudioNode::getBufferSamples()
{
    return s_pool->getBufferSize();
}

inline
int AudioNode::getBufferFrames()
{
    return s_pool->getBufferSize() / k_maxChannels;
}

inline
BufferPool* AudioNode::getBufferPool()
{
    return s_pool;
}

inline
int AudioNode::getSampleRate()
{
    return s_sampleRate;
}

}
