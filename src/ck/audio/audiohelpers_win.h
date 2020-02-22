#include "ck/core/platform.h"
#include "ck/core/types.h"


// NOTE this file should not be included directly, but only included through 
// audiohelpers_win7.cpp or audiohelpers_win8.cpp
#ifndef CK_WINVER
//#  error "windows version not defined"
#define CK_WINVER
#endif


namespace Cki
{


namespace CK_CONCAT_VALUE(AudioHelpers, CK_WINVER)
{
    bool init();
    void shutdown();

    typedef void (*BufferEndCallback)(void* data);
    void createVoice(BufferEndCallback cb, void* data);
    void destroyVoice();
    void startVoice();
    void stopVoice();
    void submitVoiceBuffer(float* buf, int bufSize);
}


}
