#include "ck/audio/audionode.h"
#include "ck/core/debug.h"
#include "ck/core/system.h"
#include "ck/ck.h"

#if CK_PLATFORM_ANDROID
#  include "ck/audio/audio_android.h"
#elif CK_PLATFORM_IOS || CK_PLATFORM_TVOS || CK_PLATFORM_OSX
#  include "ck/audio/audio_ios.h"
#endif

// template instantiations
#include "ck/core/list.cpp"

namespace Cki
{


void AudioNode::init()
{
    CK_ASSERT(!s_pool);

#if CK_PLATFORM_ANDROID
    // match output sample rate to native sample rate to avoid clicking
    // https://groups.google.com/d/topic/android-ndk/0Gtzpg49ZeE/discussion
    s_sampleRate = Audio::getNativeSampleRate();
#elif CK_PLATFORM_IOS || CK_PLATFORM_TVOS || CK_PLATFORM_OSX
    // match sample rate of audio session
    s_sampleRate = Audio::getSessionSampleRate();
#endif

    float bufferMs = System::get()->getConfig().audioUpdateMs;
    int bufferFrames = (int) (s_sampleRate * bufferMs * 0.001f);
    bufferFrames *= 2; // allow space for 2x speed at 48k
    s_pool = new BufferPool(k_maxChannels * bufferFrames);
}

void AudioNode::shutdown()
{
    CK_ASSERT(s_pool);
    delete s_pool;
    s_pool = NULL;
}

AudioNode::AudioNode() 
{
}

AudioNode::~AudioNode()
{
}

template <>
BufferHandle AudioNode::process<int32>(int frames, bool post, bool& needsPost)
{
    return processFixed(frames, post, needsPost);
}

template <>
BufferHandle AudioNode::process<float>(int frames, bool post, bool& needsPost)
{
    return processFloat(frames, post, needsPost);
}

////////////////////////////////////////

BufferHandle AudioNode::acquireBuffer()
{
    return s_pool->acquire();
}

#if CK_DEBUG
void AudioNode::printImpl(int level, const char* name)
{
    for (int i = 0; i < level; ++i)
    {
        CK_PRINT("  ");
    }
    CK_PRINT("%p %s\n", this, name);
}
#endif

BufferPool* AudioNode::s_pool = NULL;
int AudioNode::s_sampleRate = k_maxSampleRate;

////////////////////////////////////////

void AudioNode::setVolumeRampTime(float ms)
{
    s_volRampTime = ms;
}

float AudioNode::getVolumeRampTime()
{
    return s_volRampTime;
}

float AudioNode::getVolumeRate()
{
    if (s_volRampTime <= 0.0f || s_volRampTime <= 1.0f / s_sampleRate)
    {
        return 10.0f; // instantaneous
    }
    else
    {
        return 1.0f / (s_volRampTime * 0.001f * s_sampleRate);
    }
}

float AudioNode::s_volRampTime = Ck_volumeRampTimeMsDefault;

////////////////////////////////////////

template class List<AudioNode>;

}
