#include "ck/audio/graphoutput_ios.h"
#include "ck/audio/audio_ios.h"
#include "ck/core/debug.h"
#include "ck/core/system.h"

namespace Cki
{


GraphOutputIos::GraphOutputIos() 
{
    Audio::setRenderCallback(renderCallback, this);
}

GraphOutputIos::~GraphOutputIos()
{
}

void GraphOutputIos::startImpl()
{
    Audio::startGraph();
}

void GraphOutputIos::stopImpl()
{
    Audio::stopGraph();
}

OSStatus GraphOutputIos::renderCallback(void* data, AudioUnitRenderActionFlags* flags, const AudioTimeStamp* timeStamp, UInt32 busNum, UInt32 numFrames, AudioBufferList* bufList)
{
    GraphOutputIos* me = (GraphOutputIos*) data;
    if (System::get()->getSampleType() == kCkSampleType_Float)
    {
        return me->doRender<float>(flags, timeStamp, busNum, numFrames, bufList);
    }
    else
    {
        return me->doRender<int32>(flags, timeStamp, busNum, numFrames, bufList);
    }
}

template <typename T>
OSStatus GraphOutputIos::doRender(AudioUnitRenderActionFlags* flags, const AudioTimeStamp* timeStamp, UInt32 busNum, UInt32 numFrames, AudioBufferList* bufList)
{
    T* buf = (T*) bufList->mBuffers[0].mData;
    const int k_numChannels = AudioNode::k_maxChannels;
    int frames = bufList->mBuffers[0].mDataByteSize / (sizeof(T) * k_numChannels);

    CK_ASSERT(frames == numFrames);
    CK_ASSERT(bufList->mNumberBuffers == 1); // should have 1 interleaved buffer
    CK_ASSERT(bufList->mBuffers[0].mDataByteSize % (sizeof(T) * k_numChannels) == 0);

    if (!render(buf, frames))
    {
        *flags = kAudioUnitRenderAction_OutputIsSilence;
    }

    return 0;
}


}
