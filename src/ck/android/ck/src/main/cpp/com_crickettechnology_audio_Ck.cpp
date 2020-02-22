#include <jni.h>
#include "ck/ck.h"
#include "ck/config.h"
#include "ck/audio/mixer.h"
#include "ck/audio/sound.h"
#include "ck/audio/bank.h"
#include "ck/audio/effect.h"
#include "ck/audio/effectbus.h"
#include "ck/core/debug.h"
#include "ck/core/javastringref_android.h"
#include "ck/core/jnienv_android.h"
#include "floatref.h"
#include "intref.h"

namespace
{
    template <typename T>
    class ProxyInfo
    {
    public:
        ProxyInfo() : 
            m_class(NULL), 
            m_onNativeCreate(NULL), 
            m_onNativeDestroy(NULL) 
        {}

        void init(JNIEnv* jni, const char* name)
        {
            Cki::Proxied<T>::setProxyCallbacks(createCallback, destroyCallback, this);

            m_class = (jclass) jni->NewGlobalRef(jni->FindClass(name));
            m_onNativeCreate = jni->GetStaticMethodID(m_class, "onNativeCreate", "(J)V");
            m_onNativeDestroy = jni->GetStaticMethodID(m_class, "onNativeDestroy", "(J)V");
        }

        void shutdown()
        {
            Cki::Proxied<T>::setProxyCallbacks(NULL, NULL, this);

            Cki::JniEnv jni;
            jni->DeleteGlobalRef(m_class);
        }

    private:
        static void createCallback(T* inst, void* data)
        {
            ProxyInfo<T>* info = (ProxyInfo<T>*) data;
            info->create(inst);
        }

        void create(T* inst)
        {
            Cki::JniEnv jni;
            jni->CallStaticVoidMethod(m_class, m_onNativeCreate, (jlong) inst);
        }

        static void destroyCallback(T* inst, void* data)
        {
            ProxyInfo<T>* info = (ProxyInfo<T>*) data;
            info->destroy(inst);
        }

        void destroy(T* inst)
        {
            Cki::JniEnv jni;
            jni->CallStaticVoidMethod(m_class, m_onNativeDestroy, (jlong) inst);
        }

        jclass m_class;
        jmethodID m_onNativeCreate;
        jmethodID m_onNativeDestroy;
    };

    ProxyInfo<Cki::Mixer> g_mixerProxyInfo;
    ProxyInfo<Cki::Sound> g_soundProxyInfo;
    ProxyInfo<Cki::Bank> g_bankProxyInfo;
    ProxyInfo<Cki::EffectBus> g_effectBusProxyInfo;
    ProxyInfo<Cki::Effect> g_effectProxyInfo;
}

extern "C"
{


void Java_com_crickettechnology_audio_Ck_nativeInit(JNIEnv* env, jclass, jobject contextObj, jobject configObj)
{
    CkConfig config(env, contextObj);


    // copy java Config to native CkConfig
    jclass configClass = env->GetObjectClass(configObj);

    jfieldID useJavaAudioId = env->GetFieldID(configClass, "UseJavaAudio", "Z");
    config.useJavaAudio = env->GetBooleanField(configObj, useJavaAudioId);

    jfieldID enableOggVorbisId = env->GetFieldID(configClass, "EnableOggVorbis", "Z");
    config.enableOggVorbis = env->GetBooleanField(configObj, enableOggVorbisId);

    // TODO allocFunc, freeFunc, logFunc

    jfieldID logMaskId = env->GetFieldID(configClass, "LogMask", "I");
    config.logMask = env->GetIntField(configObj, logMaskId);

    jfieldID audioUpdateMsId = env->GetFieldID(configClass, "AudioUpdateMs", "F");
    config.audioUpdateMs = env->GetFloatField(configObj, audioUpdateMsId);

    jfieldID streamBufferMsId = env->GetFieldID(configClass, "StreamBufferMs", "F");
    config.streamBufferMs = env->GetFloatField(configObj, streamBufferMsId);

    jfieldID streamFileUpdateMsId = env->GetFieldID(configClass, "StreamFileUpdateMs", "F");
    config.streamFileUpdateMs = env->GetFloatField(configObj, streamFileUpdateMsId);

    jfieldID maxAudioTasksId = env->GetFieldID(configClass, "MaxAudioTasks", "I");
    config.maxAudioTasks = env->GetIntField(configObj, maxAudioTasksId);

    jfieldID maxRenderLoadId = env->GetFieldID(configClass, "MaxRenderLoad", "F");
    config.maxRenderLoad = env->GetFloatField(configObj, maxRenderLoadId);


    // set up callbacks and JNI stuff to maintain proxies
    g_mixerProxyInfo.init(env, "com/crickettechnology/audio/Mixer");
    g_soundProxyInfo.init(env, "com/crickettechnology/audio/Sound");
    g_bankProxyInfo.init(env, "com/crickettechnology/audio/Bank");
    g_effectBusProxyInfo.init(env, "com/crickettechnology/audio/EffectBus");
    g_effectProxyInfo.init(env, "com/crickettechnology/audio/Effect");

    Cki::FloatRef::init(env);
    Cki::IntRef::init(env);

    CkInit(&config);
}

void Java_com_crickettechnology_audio_Ck_nativeShutdown(JNIEnv* env, jclass)
{
    g_mixerProxyInfo.shutdown();
    g_soundProxyInfo.shutdown();
    g_bankProxyInfo.shutdown();
    g_effectBusProxyInfo.shutdown();
    g_effectProxyInfo.shutdown();

    CkShutdown();
}

void Java_com_crickettechnology_audio_Ck_nativeUpdate(JNIEnv*, jclass)
{
    CkUpdate();
}

void Java_com_crickettechnology_audio_Ck_nativeSuspend(JNIEnv*, jclass)
{
    CkSuspend();
}

void Java_com_crickettechnology_audio_Ck_nativeResume(JNIEnv*, jclass)
{
    CkResume();
}

jfloat Java_com_crickettechnology_audio_Ck_nativeGetRenderLoad(JNIEnv*, jclass)
{
    return CkGetRenderLoad();
}

jboolean Java_com_crickettechnology_audio_Ck_nativeGetClipFlag(JNIEnv*, jclass)
{
    return CkGetClipFlag();
}

void Java_com_crickettechnology_audio_Ck_nativeResetClipFlag(JNIEnv*, jclass)
{
    CkResetClipFlag();
}

void Java_com_crickettechnology_audio_Ck_nativeSetVolumeRampTime(JNIEnv*, jclass, jfloat ms)
{
    CkSetVolumeRampTime(ms);
}

jfloat Java_com_crickettechnology_audio_Ck_nativeGetVolumeRampTime(JNIEnv*, jclass)
{
    return CkGetVolumeRampTime();
}

void Java_com_crickettechnology_audio_Ck_nativeLockAudio(JNIEnv*, jclass)
{
    CkLockAudio();
}

void Java_com_crickettechnology_audio_Ck_nativeUnlockAudio(JNIEnv*, jclass)
{
    CkUnlockAudio();
}

void Java_com_crickettechnology_audio_Ck_nativeStartCapture(JNIEnv* env, jclass, jstring pathStr, int pathType)
{
    Cki::JavaStringRef path(env, pathStr);
    CkStartCapture(path.getChars(), (CkPathType) pathType);
}

void Java_com_crickettechnology_audio_Ck_nativeStopCapture(JNIEnv*, jclass)
{
    CkStopCapture();
}


}

