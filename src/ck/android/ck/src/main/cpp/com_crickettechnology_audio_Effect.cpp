#include <jni.h>
#include "ck/effect.h"

extern "C"
{


void Java_com_crickettechnology_audio_Effect_nativeSetParam__JIF(JNIEnv* env, jclass, jlong inst, jint paramId, jfloat value)
{
    CkEffect* effect = (CkEffect*) inst;
    effect->setParam(paramId, value);
}

void Java_com_crickettechnology_audio_Effect_nativeReset(JNIEnv* env, jclass, jlong inst)
{
    CkEffect* effect = (CkEffect*) inst;
    effect->reset();
}

void Java_com_crickettechnology_audio_Effect_nativeSetBypassed(JNIEnv* env, jclass, jlong inst, jboolean bypass)
{
    CkEffect* effect = (CkEffect*) inst;
    effect->setBypassed(bypass);
}

jboolean Java_com_crickettechnology_audio_Effect_nativeIsBypassed(JNIEnv* env, jclass, jlong inst)
{
    CkEffect* effect = (CkEffect*) inst;
    return effect->isBypassed();
}

void Java_com_crickettechnology_audio_Effect_nativeSetWetDryRatio(JNIEnv* env, jclass, jlong inst, jfloat wetDry)
{
    CkEffect* effect = (CkEffect*) inst;
    effect->setWetDryRatio(wetDry);
}

jfloat Java_com_crickettechnology_audio_Effect_nativeGetWetDryRatio(JNIEnv* env, jclass, jlong inst)
{
    CkEffect* effect = (CkEffect*) inst;
    return effect->getWetDryRatio();
}

jlong Java_com_crickettechnology_audio_Effect_nativeNewEffect(JNIEnv* env, jclass, jint type)
{
    CkEffect* effect = CkEffect::newEffect((CkEffectType) type);
    return (jlong) effect;
}

jlong Java_com_crickettechnology_audio_Effect_nativeNewCustomEffect(JNIEnv* env, jclass, jint id)
{
    CkEffect* effect = CkEffect::newCustomEffect((int) id);
    return (jlong) effect;
}

void Java_com_crickettechnology_audio_Effect_nativeDestroy(JNIEnv*, jclass, jlong inst)
{
    CkEffect* effect = (CkEffect*) inst;
    effect->destroy();
}


}

