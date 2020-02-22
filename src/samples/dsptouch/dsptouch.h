#pragma once

#include "ck/platform.h"
#ifdef CK_PLATFORM_ANDROID
#  include <jni.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CK_PLATFORM_ANDROID
void dspTouchInit(JNIEnv* env, jobject activity);
#else
void dspTouchInit();
#endif
void dspTouchUpdate();
void dspTouchShutdown();
void dspTouchSuspend();
void dspTouchResume();

void dspTouchStartEffect();
void dspTouchSetEffectParams(float x, float y);
void dspTouchStopEffect();

#ifdef __cplusplus
}
#endif
