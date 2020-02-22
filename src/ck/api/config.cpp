#include "ck/config.h"
#include <stddef.h>
#if CK_PLATFORM_IOS || CK_PLATFORM_TVOS
#  include <AudioToolbox/AudioToolbox.h>
#endif

#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
// seems to require multiples of 2.6667ms; 5.5 rounds down to 5.3333
const float CkConfig_audioUpdateMsDefault = 5.5f;
#else
const float CkConfig_audioUpdateMsDefault = 5.0f;
#endif
const float CkConfig_streamBufferMsDefault = 500.0f;
const float CkConfig_streamFileUpdateMsDefault = 100.0f;
const int CkConfig_maxAudioTasksDefault = 500;
const float CkConfig_maxRenderLoadDefault = 0.8f;

extern "C"
#if CK_PLATFORM_ANDROID
void CkConfigInit(CkConfig* config, JNIEnv* env, JavaVM* vm, jobject context)
#else
void CkConfigInit(CkConfig* config)
#endif
{
#if CK_PLATFORM_ANDROID
    config->jni = env;
    config->vm = vm;
    config->context = context;
    config->useJavaAudio = false;
    config->enableOggVorbis = true;
#endif
#if CK_PLATFORM_IOS || CK_PLATFORM_TVOS
    config->enableHardwareDecoding = false;
#endif
    config->allocFunc = NULL;
    config->freeFunc = NULL;
    config->logMask = kCkLog_All;
    config->logFunc = NULL;
    config->audioUpdateMs = CkConfig_audioUpdateMsDefault;
    config->streamBufferMs = CkConfig_streamBufferMsDefault;
    config->streamFileUpdateMs = CkConfig_streamFileUpdateMsDefault;
    config->maxAudioTasks = CkConfig_maxAudioTasksDefault;
    config->maxRenderLoad = CkConfig_maxRenderLoadDefault;
    config->sampleType = kCkSampleType_Default;
}


#if CK_PLATFORM_ANDROID
_CkConfig::_CkConfig(JNIEnv* env, jobject context)
{
    CkConfigInit(this, env, NULL, context);
}
_CkConfig::_CkConfig(JavaVM* vm, jobject context)
{
    CkConfigInit(this, NULL, vm, context);
}
#else
_CkConfig::_CkConfig()
{
    CkConfigInit(this);
}
#endif



