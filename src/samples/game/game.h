#pragma once

#include "ck/platform.h"
#ifdef CK_PLATFORM_ANDROID
#  include <jni.h>
#endif

typedef enum 
{
    GameTouchDown = 0,
    GameTouchMove = 1,
    GameTouchUp = 2
} GameTouchType;

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CK_PLATFORM_ANDROID
void gameInit(JNIEnv* env, jobject activity);
#else
void gameInit();
#endif
void gameSuspend();
void gameResume();
void gameShutdown();

void gameGlInit();
void gameGlShutdown();
void gameResize(int w, int h);
void gameTouch(GameTouchType type, float x, float y);
void gameRender();

void gamePrintf(const char* format, ...);

#ifdef __cplusplus
}
#endif
