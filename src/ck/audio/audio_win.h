#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

namespace Cki
{


namespace Audio
{
    typedef void (*BufferEndCallback)(void* data);
    void createVoice(BufferEndCallback cb, void* data);
    void destroyVoice();
    void startVoice();
    void stopVoice();
    void submitVoiceBuffer(float* buf, int bufSize);
}


}
