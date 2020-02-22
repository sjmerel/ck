#include <jni.h>
#include "../../../../../dsptouch.h"

extern "C"
{

void 
Java_com_crickettechnology_samples_DspTouch_DspTouchActivity_dspTouchInit(JNIEnv* env, jobject activity)
{
    dspTouchInit(env, activity);
}

void 
Java_com_crickettechnology_samples_DspTouch_DspTouchActivity_dspTouchUpdate(JNIEnv* env, jobject activity)
{
    dspTouchUpdate();
}

void 
Java_com_crickettechnology_samples_DspTouch_DspTouchActivity_dspTouchShutdown(JNIEnv* env, jobject activity)
{
    dspTouchShutdown();
}

void 
Java_com_crickettechnology_samples_DspTouch_DspTouchActivity_dspTouchSuspend(JNIEnv* env, jobject activity)
{
    dspTouchSuspend();
}

void 
Java_com_crickettechnology_samples_DspTouch_DspTouchActivity_dspTouchResume(JNIEnv* env, jobject activity)
{
    dspTouchResume();
}

void 
Java_com_crickettechnology_samples_DspTouch_DspTouchActivity_dspTouchStartEffect(JNIEnv* env, jobject activity)
{
    dspTouchStartEffect();
}

void 
Java_com_crickettechnology_samples_DspTouch_DspTouchActivity_dspTouchSetEffectParams(JNIEnv* env, jobject activity, jfloat x, jfloat y)
{
    dspTouchSetEffectParams(x, y);
}

void 
Java_com_crickettechnology_samples_DspTouch_DspTouchActivity_dspTouchStopEffect(JNIEnv* env, jobject activity)
{
    dspTouchStopEffect();
}


}

