#include <jni.h>
#include "ck/sound.h"
#include "ck/core/javastringref_android.h"
#include "floatref.h"
#include "intref.h"

extern "C"
{


void Java_com_crickettechnology_audio_Sound_nativeSetMixer(JNIEnv* env, jclass, jlong inst, jlong mixerInst)
{
    CkSound* sound = (CkSound*) inst;
    sound->setMixer((CkMixer*) mixerInst);
}

jlong Java_com_crickettechnology_audio_Sound_nativeGetMixer(JNIEnv* env, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jlong) sound->getMixer();
}

jboolean Java_com_crickettechnology_audio_Sound_nativeIsReady(JNIEnv* env, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return sound->isReady();
}

jboolean Java_com_crickettechnology_audio_Sound_nativeIsFailed(JNIEnv* env, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return sound->isFailed();
}

void Java_com_crickettechnology_audio_Sound_nativePlay(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    sound->play();
}

void Java_com_crickettechnology_audio_Sound_nativeStop(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    sound->stop();
}

jboolean Java_com_crickettechnology_audio_Sound_nativeIsPlaying(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jboolean) sound->isPlaying();
}

void Java_com_crickettechnology_audio_Sound_nativeSetPaused(JNIEnv*, jclass, jlong inst, jboolean paused)
{
    CkSound* sound = (CkSound*) inst;
    sound->setPaused(paused);
}

jboolean Java_com_crickettechnology_audio_Sound_nativeIsPaused(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jboolean) sound->isPaused();
}

jboolean Java_com_crickettechnology_audio_Sound_nativeGetMixedPauseState(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jboolean) sound->getMixedPauseState();
}

void Java_com_crickettechnology_audio_Sound_nativeSetLoop(JNIEnv*, jclass, jlong inst, jint loopStart, jint loopEnd)
{
    CkSound* sound = (CkSound*) inst;
    sound->setLoop(loopStart, loopEnd);
}

void Java_com_crickettechnology_audio_Sound_nativeGetLoop(JNIEnv* jni, jclass, jlong inst, jobject loopStartObj, jobject loopEndObj)
{
    CkSound* sound = (CkSound*) inst;
    int loopStart, loopEnd;
    sound->getLoop(loopStart, loopEnd);

    Cki::IntRef::set(jni, loopStartObj, loopStart);
    Cki::IntRef::set(jni, loopEndObj, loopEnd);
}

void Java_com_crickettechnology_audio_Sound_nativeSetLoopCount(JNIEnv*, jclass, jlong inst, jint loopCount)
{
    CkSound* sound = (CkSound*) inst;
    sound->setLoopCount(loopCount);
}

jint Java_com_crickettechnology_audio_Sound_nativeGetLoopCount(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jint) sound->getLoopCount();
}

jint Java_com_crickettechnology_audio_Sound_nativeGetCurrentLoop(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jint) sound->getCurrentLoop();
}

void Java_com_crickettechnology_audio_Sound_nativeReleaseLoop(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    sound->releaseLoop();
}

jboolean Java_com_crickettechnology_audio_Sound_nativeIsLoopReleased(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return sound->isLoopReleased();
}

void Java_com_crickettechnology_audio_Sound_nativeSetPlayPosition(JNIEnv*, jclass, jlong inst, jint frame)
{
    CkSound* sound = (CkSound*) inst;
    sound->setPlayPosition(frame);
}

void Java_com_crickettechnology_audio_Sound_nativeSetPlayPositionMs(JNIEnv*, jclass, jlong inst, jfloat ms)
{
    CkSound* sound = (CkSound*) inst;
    sound->setPlayPositionMs(ms);
}

jint Java_com_crickettechnology_audio_Sound_nativeGetPlayPosition(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jint) sound->getPlayPosition();
}

jfloat Java_com_crickettechnology_audio_Sound_nativeGetPlayPositionMs(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jfloat) sound->getPlayPositionMs();
}

void Java_com_crickettechnology_audio_Sound_nativeSetVolume(JNIEnv*, jclass, jlong inst, jfloat volume)
{
    CkSound* sound = (CkSound*) inst;
    sound->setVolume(volume);
}

jfloat Java_com_crickettechnology_audio_Sound_nativeGetVolume(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jfloat) sound->getVolume();
}

jfloat Java_com_crickettechnology_audio_Sound_nativeGetMixedVolume(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jfloat) sound->getMixedVolume();
}

void Java_com_crickettechnology_audio_Sound_nativeSetPan(JNIEnv*, jclass, jlong inst, jfloat pan)
{
    CkSound* sound = (CkSound*) inst;
    sound->setPan(pan);
}

jfloat Java_com_crickettechnology_audio_Sound_nativeGetPan(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jfloat) sound->getPan();
}

void Java_com_crickettechnology_audio_Sound_nativeSetPanMatrix(JNIEnv*, jclass, jlong inst, jfloat ll, jfloat lr, jfloat rl, jfloat rr)
{
    CkSound* sound = (CkSound*) inst;
    sound->setPanMatrix(ll, lr, rl, rr);
}

void Java_com_crickettechnology_audio_Sound_nativeGetPanMatrix(JNIEnv* jni, jclass, jlong inst, jobject llObj, jobject lrObj, jobject rlObj, jobject rrObj)
{
    CkSound* sound = (CkSound*) inst;
    float ll, lr, rl, rr;
    sound->getPanMatrix(ll, lr, rl, rr);

    Cki::FloatRef::set(jni, llObj, ll);
    Cki::FloatRef::set(jni, lrObj, lr);
    Cki::FloatRef::set(jni, rlObj, rl);
    Cki::FloatRef::set(jni, rrObj, rr);
}

void Java_com_crickettechnology_audio_Sound_nativeSetPitchShift(JNIEnv*, jclass, jlong inst, jfloat halfSteps)
{
    CkSound* sound = (CkSound*) inst;
    sound->setPitchShift(halfSteps);
}

jfloat Java_com_crickettechnology_audio_Sound_nativeGetPitchShift(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jfloat) sound->getPitchShift();
}

void Java_com_crickettechnology_audio_Sound_nativeSetSpeed(JNIEnv*, jclass, jlong inst, jfloat speed)
{
    CkSound* sound = (CkSound*) inst;
    sound->setSpeed(speed);
}

jfloat Java_com_crickettechnology_audio_Sound_nativeGetSpeed(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jfloat) sound->getSpeed();
}

void Java_com_crickettechnology_audio_Sound_nativeSetNextSound(JNIEnv*, jclass, jlong inst, jlong nextInst)
{
    CkSound* sound = (CkSound*) inst;
    sound->setNextSound((CkSound*) nextInst);
}

jlong Java_com_crickettechnology_audio_Sound_nativeGetNextSound(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jlong) sound->getNextSound();
}

jint Java_com_crickettechnology_audio_Sound_nativeGetLength(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jint) sound->getLength();
}

jfloat Java_com_crickettechnology_audio_Sound_nativeGetLengthMs(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jfloat) sound->getLengthMs();
}

jint Java_com_crickettechnology_audio_Sound_nativeGetSampleRate(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jint) sound->getSampleRate();
}

jint Java_com_crickettechnology_audio_Sound_nativeGetChannels(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jint) sound->getChannels();
}

void Java_com_crickettechnology_audio_Sound_nativeSetEffectBus(JNIEnv*, jclass, jlong inst, jlong busInst)
{
    CkSound* sound = (CkSound*) inst;
    sound->setEffectBus((CkEffectBus*) busInst);
}

jlong Java_com_crickettechnology_audio_Sound_nativeGetEffectBus(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jlong) sound->getEffectBus();
}

void Java_com_crickettechnology_audio_Sound_nativeSet3dEnabled(JNIEnv*, jclass, jlong inst, jboolean enabled)
{
    CkSound* sound = (CkSound*) inst;
    sound->set3dEnabled(enabled);
}

jboolean Java_com_crickettechnology_audio_Sound_nativeIs3dEnabled(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jboolean) sound->is3dEnabled();
}

jboolean Java_com_crickettechnology_audio_Sound_nativeIsVirtual(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    return (jboolean) sound->isVirtual();
}

void Java_com_crickettechnology_audio_Sound_nativeSet3dPosition(JNIEnv*, jclass, jlong inst, jfloat x, jfloat y, jfloat z)
{
    CkSound* sound = (CkSound*) inst;
    sound->set3dPosition(x, y, z);
}

void Java_com_crickettechnology_audio_Sound_nativeGet3dPosition(JNIEnv* jni, jclass, jlong inst, jobject xObj, jobject yObj, jobject zObj)
{
    CkSound* sound = (CkSound*) inst;
    float x, y, z;
    sound->get3dPosition(x, y, z);

    Cki::FloatRef::set(jni, xObj, x);
    Cki::FloatRef::set(jni, yObj, y);
    Cki::FloatRef::set(jni, zObj, z);
}

void Java_com_crickettechnology_audio_Sound_nativeSet3dVelocity(JNIEnv*, jclass, jlong inst, jfloat x, jfloat y, jfloat z)
{
    CkSound* sound = (CkSound*) inst;
    sound->set3dVelocity(x, y, z);
}

void Java_com_crickettechnology_audio_Sound_nativeGet3dVelocity(JNIEnv* jni, jclass, jlong inst, jobject xObj, jobject yObj, jobject zObj)
{
    CkSound* sound = (CkSound*) inst;
    float x, y, z;
    sound->get3dVelocity(x, y, z);

    Cki::FloatRef::set(jni, xObj, x);
    Cki::FloatRef::set(jni, yObj, y);
    Cki::FloatRef::set(jni, zObj, z);
}

void Java_com_crickettechnology_audio_Sound_nativeSet3dListenerPosition(JNIEnv*, jclass, 
        jfloat eyeX, jfloat eyeY, jfloat eyeZ,
        jfloat lookAtX, jfloat lookAtY, jfloat lookAtZ,
        jfloat upX, jfloat upY, jfloat upZ)
{
    CkSound::set3dListenerPosition(
            eyeX, eyeY, eyeZ,
            lookAtX, lookAtY, lookAtZ,
            upX, upY, upZ);
}

void Java_com_crickettechnology_audio_Sound_nativeGet3dListenerPosition(JNIEnv* jni, jclass, 
        jobject eyeXObj, jobject eyeYObj, jobject eyeZObj,
        jobject lookAtXObj, jobject lookAtYObj, jobject lookAtZObj,
        jobject upXObj, jobject upYObj, jobject upZObj)
{
    float eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ;
    CkSound::get3dListenerPosition(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);

    Cki::FloatRef::set(jni, eyeXObj, eyeX);
    Cki::FloatRef::set(jni, eyeYObj, eyeY);
    Cki::FloatRef::set(jni, eyeZObj, eyeZ);
    Cki::FloatRef::set(jni, lookAtXObj, lookAtX);
    Cki::FloatRef::set(jni, lookAtYObj, lookAtY);
    Cki::FloatRef::set(jni, lookAtZObj, lookAtZ);
    Cki::FloatRef::set(jni, upXObj, upX);
    Cki::FloatRef::set(jni, upYObj, upY);
    Cki::FloatRef::set(jni, upZObj, upZ);
}

void Java_com_crickettechnology_audio_Sound_nativeSet3dListenerVelocity(JNIEnv*, jclass, 
        jfloat x, jfloat y, jfloat z)
{
    CkSound::set3dListenerVelocity(x, y, z);
}

void Java_com_crickettechnology_audio_Sound_nativeGet3dListenerVelocity(JNIEnv* jni, jclass, 
        jobject xObj, jobject yObj, jobject zObj)
{
    float x, y, z;
    CkSound::get3dListenerVelocity(x, y, z);

    Cki::FloatRef::set(jni, xObj, x);
    Cki::FloatRef::set(jni, yObj, y);
    Cki::FloatRef::set(jni, zObj, z);
}

void Java_com_crickettechnology_audio_Sound_nativeSet3dAttenuation(JNIEnv*, jclass, jint mode, jfloat nearDist, jfloat farDist, jfloat farVol)
{
    CkSound::set3dAttenuation((CkAttenuationMode) mode, nearDist, farDist, farVol);
}

void Java_com_crickettechnology_audio_Sound_nativeGet3dAttenuation(JNIEnv* jni, jclass, jobject modeObj, jobject nearDistObj, jobject farDistObj, jobject farVolObj)
{
    CkAttenuationMode mode;
    float nearDist, farDist, farVol;
    CkSound::get3dAttenuation(mode, nearDist, farDist, farVol);

    Cki::IntRef::set(jni, modeObj, (int) mode);
    Cki::FloatRef::set(jni, nearDistObj, nearDist);
    Cki::FloatRef::set(jni, farDistObj, farDist);
    Cki::FloatRef::set(jni, farVolObj, farVol);
}

void Java_com_crickettechnology_audio_Sound_nativeSet3dSoundSpeed(JNIEnv*, jclass, jfloat speed)
{
    CkSound::set3dSoundSpeed(speed);
}

jfloat Java_com_crickettechnology_audio_Sound_nativeGet3dSoundSpeed(JNIEnv* jni, jclass)
{
    return CkSound::get3dSoundSpeed();
}

jlong Java_com_crickettechnology_audio_Sound_nativeNewBankSoundByIndex(JNIEnv* env, jclass, jlong bankInst, jint index)
{
    CkBank* bank = (CkBank*) bankInst;
    CkSound* sound = CkSound::newBankSound(bank, index);
    return (jlong) sound;
}

jlong Java_com_crickettechnology_audio_Sound_nativeNewBankSoundByName(JNIEnv* env, jclass, jlong bankInst, jstring nameStr)
{
    CkBank* bank = (CkBank*) bankInst;
    Cki::JavaStringRef name(env, nameStr);
    CkSound* sound = CkSound::newBankSound(bank, name.getChars());
    return (jlong) sound;
}

jlong Java_com_crickettechnology_audio_Sound_nativeNewStreamSound(JNIEnv* env, jclass, jstring pathStr, jint pathType, jint offset, jint length, jstring extensionStr)
{
    Cki::JavaStringRef path(env, pathStr);
    Cki::JavaStringRef extension(env, extensionStr);
    CkSound* sound = CkSound::newStreamSound(path.getChars(), (CkPathType) pathType, offset, length, extension.getChars());
    return (jlong) sound;
}

void Java_com_crickettechnology_audio_Sound_nativeDestroy(JNIEnv*, jclass, jlong inst)
{
    CkSound* sound = (CkSound*) inst;
    sound->destroy();
}


}
