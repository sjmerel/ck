#include <jni.h>
#include "../../../../../game.h"

extern "C"
{

void
Java_com_crickettechnology_game_GameActivity_nativeCreate(JNIEnv* env, jclass, jobject activity)
{
    gamePrintf("game create\n");
    gameInit(env, activity);
}

void
Java_com_crickettechnology_game_GameRenderer_nativeGlInit(JNIEnv* env, jclass)
{
    gamePrintf("game GL init\n");
    gameGlInit();
}

void
Java_com_crickettechnology_game_GameRenderer_nativeResize(JNIEnv* env, jclass, jint w, jint h)
{
    gamePrintf("game resize\n");
    gameResize(w, h);
}

void
Java_com_crickettechnology_game_GameActivity_nativePause(JNIEnv* env, jclass, jint w, jint h)
{
    gamePrintf("game suspend\n");
    gameSuspend();
}

void
Java_com_crickettechnology_game_GameActivity_nativeResume(JNIEnv* env, jclass, jint w, jint h)
{
    gamePrintf("game resume\n");
    gameResume();
}

void
Java_com_crickettechnology_game_GameActivity_nativeDestroy(JNIEnv* env, jclass)
{
    gamePrintf("game destroy\n");
    gameShutdown();
}

void
Java_com_crickettechnology_game_GameRenderer_nativeRender(JNIEnv* env, jclass)
{
    gameRender();
}

void
Java_com_crickettechnology_game_GameGLSurfaceView_nativeTouch(JNIEnv* env, jclass, jint code, jfloat x, jfloat y)
{
    gameTouch((GameTouchType) code, x, y);
}

}
