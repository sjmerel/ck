#include "ck.h"
#include "bank.h"
#include "proxy.h"
#include "config.h"
#include "stringconvert.h"
#include "ck/ck.h"
#include <windows.h>

using namespace Platform;

namespace CricketTechnology
{
namespace Audio
{

int Ck::Init(Config^ config)
{
    Proxy::Init();

    return CkInit(config->Impl);
}

void Ck::Update()
{
    CkUpdate();
}

void Ck::Shutdown()
{
    CkShutdown();

    Proxy::Shutdown();
}

void Ck::Suspend()
{
    CkSuspend();
}

void Ck::Resume()
{
    CkResume();
}

float Ck::RenderLoad::get()
{
    return CkGetRenderLoad();
}

bool Ck::ClipFlag::get()
{
    return CkGetClipFlag();
}

void Ck::ResetClipFlag()
{
    CkResetClipFlag();
}

float Ck::VolumeRampTime::get()
{
    return CkGetVolumeRampTime();
}

void Ck::VolumeRampTime::set(float ms)
{
    CkSetVolumeRampTime(ms);
}

float Ck::VolumeRampTimeMsDefault::get()
{
    return Ck_volumeRampTimeMsDefault;
}

void Ck::LockAudio()
{
    CkLockAudio();
}

void Ck::UnlockAudio()
{
    CkUnlockAudio();
}

void Ck::StartCapture(Platform::String^ path, PathType pathType)
{
    StringConvert<256> convert(path);
    CkStartCapture(convert.getCString(), (CkPathType) pathType);
}

void Ck::StopCapture()
{
    CkStopCapture();
}


}
}
