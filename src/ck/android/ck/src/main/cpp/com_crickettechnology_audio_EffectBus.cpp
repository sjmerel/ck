#include <jni.h>
#include "ck/effectbus.h"

extern "C"
{


void Java_com_crickettechnology_audio_EffectBus_nativeAddEffect(JNIEnv* env, jclass, jlong inst, jlong effectInst)
{
    CkEffectBus* effectBus = (CkEffectBus*) inst;
    effectBus->addEffect((CkEffect*) effectInst);
}

void Java_com_crickettechnology_audio_EffectBus_nativeRemoveEffect(JNIEnv* env, jclass, jlong inst, jlong effectInst)
{
    CkEffectBus* effectBus = (CkEffectBus*) inst;
    effectBus->removeEffect((CkEffect*) effectInst);
}

void Java_com_crickettechnology_audio_EffectBus_nativeRemoveAllEffects(JNIEnv* env, jclass, jlong inst)
{
    CkEffectBus* effectBus = (CkEffectBus*) inst;
    effectBus->removeAllEffects();
}

void Java_com_crickettechnology_audio_EffectBus_nativeSetOutputBus(JNIEnv* env, jclass, jlong inst, jlong outputBusInst)
{
    CkEffectBus* effectBus = (CkEffectBus*) inst;
    effectBus->setOutputBus((CkEffectBus*) outputBusInst);
}

jlong Java_com_crickettechnology_audio_EffectBus_nativeGetOutputBus(JNIEnv* env, jclass, jlong inst)
{
    CkEffectBus* effectBus = (CkEffectBus*) inst;
    return (jlong) effectBus->getOutputBus();
}

void Java_com_crickettechnology_audio_EffectBus_nativeReset(JNIEnv* env, jclass, jlong inst)
{
    CkEffectBus* effectBus = (CkEffectBus*) inst;
    effectBus->reset();
}

void Java_com_crickettechnology_audio_EffectBus_nativeSetBypassed(JNIEnv* env, jclass, jlong inst, jboolean bypass)
{
    CkEffectBus* effectBus = (CkEffectBus*) inst;
    effectBus->setBypassed(bypass);
}

jboolean Java_com_crickettechnology_audio_EffectBus_nativeIsBypassed(JNIEnv* env, jclass, jlong inst)
{
    CkEffectBus* effectBus = (CkEffectBus*) inst;
    return effectBus->isBypassed();
}

void Java_com_crickettechnology_audio_EffectBus_nativeSetWetDryRatio(JNIEnv* env, jclass, jlong inst, jfloat wetDry)
{
    CkEffectBus* effectBus = (CkEffectBus*) inst;
    effectBus->setWetDryRatio(wetDry);
}

jfloat Java_com_crickettechnology_audio_EffectBus_nativeGetWetDryRatio(JNIEnv* env, jclass, jlong inst)
{
    CkEffectBus* effectBus = (CkEffectBus*) inst;
    return effectBus->getWetDryRatio();
}

jlong Java_com_crickettechnology_audio_EffectBus_nativeNewEffectBus(JNIEnv* env, jclass)
{
    CkEffectBus* effectBus = CkEffectBus::newEffectBus();
    return (jlong) effectBus;
}

jlong Java_com_crickettechnology_audio_EffectBus_nativeGetGlobalEffectBus(JNIEnv* env, jclass)
{
    return (jlong) CkEffectBus::getGlobalEffectBus();
}

void Java_com_crickettechnology_audio_EffectBus_nativeDestroy(JNIEnv*, jclass, jlong inst)
{
    CkEffectBus* effectBus = (CkEffectBus*) inst;
    effectBus->destroy();
}


}


