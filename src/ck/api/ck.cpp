#include "ck/ck.h"
#include "ck/core/system.h"
#include "ck/core/logger.h"
#include "ck/core/debug.h"
#include "ck/audio/audiograph.h"
#include "ck/audio/audionode.h"


#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
#  include "ck/api/objc/cko.h"
#endif

using namespace Cki;

extern "C"
{

int CkInit(CkConfig* config)
{
    bool success = true;

    if (!System::get())
    {

#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
        CkoInit();
#endif
        success &= System::init(*config);
    }

    if (!success)
    {
        CK_LOG_ERROR("CkInit FAILED!");
        CkShutdown();
    }

    return (success ? -1 : 0);
}

void CkUpdate()
{ 
    System* system = System::get();
    if (!system)
    {
        CK_LOG_ERROR("You must call CkInit() before calling CkUpdate()!");
    }
    else
    {
        system->update();
    }
}

void CkShutdown()
{
    if (System::get())
    {
        System::shutdown();
#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
        CkoShutdown();
#endif
    }
}

void CkSuspend()
{ 
    System* system = System::get();
    if (!system)
    {
        CK_LOG_ERROR("You must call CkInit() before calling CkSuspend()!");
    }
    else
    {
        system->suspend();
    }
}

void CkResume()
{ 
    System* system = System::get();
    if (!system)
    {
        CK_LOG_ERROR("You must call CkInit() before calling CkResume()!");
    }
    else
    {
        system->resume();
    }
}

float CkGetRenderLoad()
{
    return AudioGraph::get()->getRenderLoad();
}

int CkGetClipFlag()
{
    return AudioGraph::get()->getClipFlag();
}

void CkResetClipFlag()
{
    AudioGraph::get()->resetClipFlag();
}

void CkSetVolumeRampTime(float ms)
{
    AudioNode::setVolumeRampTime(ms);
}

float CkGetVolumeRampTime()
{
    return AudioNode::getVolumeRampTime();
}

const float Ck_volumeRampTimeMsDefault = 40.0f;

void CkLockAudio()
{
    AudioGraph::get()->lockAudio();
}

void CkUnlockAudio()
{
    AudioGraph::get()->unlockAudio();
}

void CkStartCapture(const char* path, CkPathType pathType)
{
    AudioGraph::get()->startCapture(path, pathType);
}

void CkStopCapture()
{
    AudioGraph::get()->stopCapture();
}

} // extern "C"
