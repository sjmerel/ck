#pragma once

#include "ck/audio/graphoutput.h"
#include <AudioToolbox/AudioToolbox.h>

namespace Cki
{


class GraphOutputIos : public GraphOutput
{
public:
    GraphOutputIos();
    virtual ~GraphOutputIos();

protected:
    virtual void startImpl();
    virtual void stopImpl();

private:
    static OSStatus renderCallback(void* data, AudioUnitRenderActionFlags* flags, const AudioTimeStamp* timeStamp, UInt32 busNum, UInt32 numFrames, AudioBufferList* bufList);
    template <typename T>
    OSStatus doRender(AudioUnitRenderActionFlags* flags, const AudioTimeStamp* timeStamp, UInt32 busNum, UInt32 numFrames, AudioBufferList* bufList);
};


}
