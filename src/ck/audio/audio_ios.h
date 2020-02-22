#pragma once

#include "ck/core/platform.h"
#include <AudioToolbox/AudioToolbox.h>

namespace Cki
{


namespace Audio
{
    // No definitive word on this from Apple docs, but most likely iOS devices
    // have 44.1k as native hardware sample rate.  iPhone 6s seems not to be able to handle 44100, though.
    enum { k_outputSampleRate = 48000 };

    void setRenderCallback(AURenderCallback callback, void* data);
    void startGraph();
    void stopGraph();

    int getSessionSampleRate();
}


}
