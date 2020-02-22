#include <jni.h>
#include "ck/mixer.h"
#include "ck/core/javastringref_android.h"

extern "C"
{


void Java_com_crickettechnology_audio_Mixer_nativeSetName(JNIEnv* env, jclass, jlong inst, jstring nameStr)
{
    const char* name = env->GetStringUTFChars(nameStr, 0);
    CkMixer* mixer = (CkMixer*) inst;
    mixer->setName(name);
    env->ReleaseStringUTFChars(nameStr, name);
}

jstring Java_com_crickettechnology_audio_Mixer_nativeGetName(JNIEnv* env, jclass, jlong inst)
{
    CkMixer* mixer = (CkMixer*) inst;
    const char* name = mixer->getName();
    return env->NewStringUTF(name);
}

void Java_com_crickettechnology_audio_Mixer_nativeSetVolume(JNIEnv* env, jclass, jlong inst, jfloat volume)
{
    CkMixer* mixer = (CkMixer*) inst;
    mixer->setVolume(volume);
}

jfloat Java_com_crickettechnology_audio_Mixer_nativeGetVolume(JNIEnv* env, jclass, jlong inst)
{
    CkMixer* mixer = (CkMixer*) inst;
    return mixer->getVolume();
}

jfloat Java_com_crickettechnology_audio_Mixer_nativeGetMixedVolume(JNIEnv* env, jclass, jlong inst)
{
    CkMixer* mixer = (CkMixer*) inst;
    return mixer->getMixedVolume();
}

void Java_com_crickettechnology_audio_Mixer_nativeSetPaused(JNIEnv* env, jclass, jlong inst, jboolean paused)
{
    CkMixer* mixer = (CkMixer*) inst;
    mixer->setPaused(paused);
}

jboolean Java_com_crickettechnology_audio_Mixer_nativeIsPaused(JNIEnv* env, jclass, jlong inst)
{
    CkMixer* mixer = (CkMixer*) inst;
    return mixer->isPaused();
}

jboolean Java_com_crickettechnology_audio_Mixer_nativeGetMixedPauseState(JNIEnv* env, jclass, jlong inst)
{
    CkMixer* mixer = (CkMixer*) inst;
    return mixer->getMixedPauseState();
}

void Java_com_crickettechnology_audio_Mixer_nativeSetParent(JNIEnv* env, jclass, jlong inst, jlong parentInst)
{
    CkMixer* mixer = (CkMixer*) inst;
    mixer->setParent((CkMixer*) parentInst);
}

jlong Java_com_crickettechnology_audio_Mixer_nativeGetParent(JNIEnv* env, jclass, jlong inst)
{
    CkMixer* mixer = (CkMixer*) inst;
    return (jlong) mixer->getParent();
}

jlong Java_com_crickettechnology_audio_Mixer_nativeGetMaster(JNIEnv* env, jclass)
{
    return (jlong) CkMixer::getMaster();
}

jlong Java_com_crickettechnology_audio_Mixer_nativeNewMixer(JNIEnv* env, jclass, jstring nameStr, jlong parentInst)
{
    Cki::JavaStringRef name(env, nameStr);
    CkMixer* mixer = CkMixer::newMixer(name.getChars(), (CkMixer*) parentInst);
    return (jlong) mixer;
}

jlong Java_com_crickettechnology_audio_Mixer_nativeFind(JNIEnv* env, jclass, jstring nameStr)
{
    Cki::JavaStringRef name(env, nameStr);
    CkMixer* mixer = CkMixer::find(name.getChars());
    return (jlong) mixer;
}

void Java_com_crickettechnology_audio_Mixer_nativeDestroy(JNIEnv*, jclass, jlong inst)
{
    CkMixer* mixer = (CkMixer*) inst;
    mixer->destroy();
}


}
