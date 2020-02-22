#include "ck/audio/audiotrackproxy_android.h"
#include "ck/core/system_android.h"
#include "ck/core/debug.h"
#include "ck/core/jnienv_android.h"

namespace Cki
{


namespace
{
    int getIntField(JNIEnv* jni, const char* className, const char* fieldName)
    {
        jclass clazz = jni->FindClass(className);
        jfieldID fieldId = jni->GetStaticFieldID(clazz, fieldName, "I");
        return jni->GetStaticIntField(clazz, fieldId);
    }
}

////////////////////////////////////////

AudioTrackProxy::AudioTrackProxy(int sampleRate, int channels, int bufFrames) :
    m_audioTrack(NULL)
{
    CK_ASSERT(s_class);

    JniEnv jni;

    // create audiotrack and global reference

    /*
    m_track = new AudioTrack(
                AudioManager.STREAM_MUSIC, // ??
                sampleRate, 
                (channels == 1 ? AudioFormat.CHANNEL_OUT_MONO : AudioFormat.CHANNEL_OUT_STEREO);
                AudioFormat.ENCODING_PCM_16BIT,
                bufFrames * channels * (Short.SIZE/8),
                AudioTrack.MODE_STREAM);
    */

    CK_ASSERT(channels == 1 || channels == 2);
    jobject audioTrack = jni->NewObject(s_class, s_ctorId, 
            s_AudioManager_STREAM_MUSIC,
            sampleRate, 
            channels == 1 ? s_AudioFormat_CHANNEL_OUT_MONO : s_AudioFormat_CHANNEL_OUT_STEREO,
            s_AudioFormat_ENCODING_PCM_16BIT,
            bufFrames * channels * sizeof(int16),
            s_AudioTrack_MODE_STREAM);
    m_audioTrack = jni->NewGlobalRef(audioTrack);
}

AudioTrackProxy::~AudioTrackProxy()
{
    JniEnv jni;
    jni->CallVoidMethod(m_audioTrack, s_releaseId);
    jni->DeleteGlobalRef(m_audioTrack);
}

void AudioTrackProxy::play()
{
    JniEnv jni;
    jni->CallVoidMethod(m_audioTrack, s_playId);
}

void AudioTrackProxy::stop()
{
    JniEnv jni;
    jni->CallVoidMethod(m_audioTrack, s_stopId);
}

void AudioTrackProxy::pause()
{
    JniEnv jni;
    jni->CallVoidMethod(m_audioTrack, s_pauseId);
}

void AudioTrackProxy::flush()
{
    JniEnv jni;
    jni->CallVoidMethod(m_audioTrack, s_flushId);
}

int AudioTrackProxy::getPlaybackHeadPosition()
{
    JniEnv jni;
    return jni->CallIntMethod(m_audioTrack, s_getPlaybackHeadPositionId);
}

void AudioTrackProxy::setPlaybackHeadPosition(int pos)
{
    JniEnv jni;
    CK_VERIFY( s_AudioTrack_SUCCESS == jni->CallIntMethod(m_audioTrack, s_setPlaybackHeadPositionId, pos) );
}

void AudioTrackProxy::setPlaybackRate(int rate)
{
    JniEnv jni;
    CK_VERIFY( s_AudioTrack_SUCCESS == jni->CallIntMethod(m_audioTrack, s_setPlaybackRateId, rate) );
}

void AudioTrackProxy::setStereoVolume(float left, float right)
{
    JniEnv jni;
    CK_VERIFY( s_AudioTrack_SUCCESS == jni->CallIntMethod(m_audioTrack, s_setStereoVolumeId, left, right) );
}

int AudioTrackProxy::write(jshortArray array, int count)
{
    JniEnv jni;
    return jni->CallIntMethod(m_audioTrack, s_writeId, array, 0, count);
}

int AudioTrackProxy::getMinBufferSize(int sampleRate, int channels)
{
    JniEnv jni;
    CK_ASSERT(channels == 1 || channels == 2);
    int channelConfig = (channels == 1 ? s_AudioFormat_CHANNEL_OUT_MONO : s_AudioFormat_CHANNEL_OUT_STEREO);
    return jni->CallStaticIntMethod(s_class, s_getMinBufferSizeId, sampleRate, channelConfig, s_AudioFormat_ENCODING_PCM_16BIT);
}

int AudioTrackProxy::getNativeOutputSampleRate()
{
    JniEnv jni;
    return jni->CallStaticIntMethod(s_class, s_getNativeOutputSampleRateId, s_AudioManager_STREAM_MUSIC);
}

////////////////////////////////////////

jclass AudioTrackProxy::s_class = NULL;

jmethodID AudioTrackProxy::s_ctorId = NULL;
jmethodID AudioTrackProxy::s_releaseId = NULL;
jmethodID AudioTrackProxy::s_playId = NULL;
jmethodID AudioTrackProxy::s_stopId = NULL;
jmethodID AudioTrackProxy::s_pauseId = NULL;
jmethodID AudioTrackProxy::s_flushId = NULL;
jmethodID AudioTrackProxy::s_getPlaybackHeadPositionId = NULL;
jmethodID AudioTrackProxy::s_setPlaybackHeadPositionId = NULL;
jmethodID AudioTrackProxy::s_setPlaybackRateId = NULL;
jmethodID AudioTrackProxy::s_setStereoVolumeId = NULL;
jmethodID AudioTrackProxy::s_writeId = NULL;
jmethodID AudioTrackProxy::s_getMinBufferSizeId = NULL;
jmethodID AudioTrackProxy::s_getNativeOutputSampleRateId = NULL;

int AudioTrackProxy::s_AudioManager_STREAM_MUSIC = 0;
int AudioTrackProxy::s_AudioFormat_ENCODING_PCM_16BIT = 0;
int AudioTrackProxy::s_AudioFormat_CHANNEL_OUT_MONO = 0;
int AudioTrackProxy::s_AudioFormat_CHANNEL_OUT_STEREO = 0;
int AudioTrackProxy::s_AudioTrack_MODE_STREAM = 0;
int AudioTrackProxy::s_AudioTrack_SUCCESS = 0;

////////////////////////////////////////

void AudioTrackProxy::init()
{
    CK_ASSERT(!s_class);

    JniEnv jni;

    jclass cls = jni->FindClass("android/media/AudioTrack");
    s_class = (jclass) jni->NewGlobalRef(cls);

    s_ctorId = jni->GetMethodID(s_class, "<init>", "(IIIIII)V");
    s_releaseId = jni->GetMethodID(s_class, "release", "()V");
    s_playId = jni->GetMethodID(s_class, "play", "()V");
    s_stopId = jni->GetMethodID(s_class, "stop", "()V");
    s_pauseId = jni->GetMethodID(s_class, "pause", "()V");
    s_flushId = jni->GetMethodID(s_class, "flush", "()V");
    s_getPlaybackHeadPositionId = jni->GetMethodID(s_class, "getPlaybackHeadPosition", "()I");
    s_setPlaybackHeadPositionId = jni->GetMethodID(s_class, "setPlaybackHeadPosition", "(I)I");
    s_setPlaybackRateId = jni->GetMethodID(s_class, "setPlaybackRate", "(I)I");
    s_setStereoVolumeId = jni->GetMethodID(s_class, "setStereoVolume", "(FF)I");
    s_writeId = jni->GetMethodID(s_class, "write", "([SII)I");
    s_getMinBufferSizeId = jni->GetStaticMethodID(s_class, "getMinBufferSize", "(III)I");
    s_getNativeOutputSampleRateId = jni->GetStaticMethodID(s_class, "getNativeOutputSampleRate", "(I)I");

    s_AudioManager_STREAM_MUSIC = getIntField(jni, "android/media/AudioManager", "STREAM_MUSIC");
    s_AudioFormat_ENCODING_PCM_16BIT = getIntField(jni, "android/media/AudioFormat", "ENCODING_PCM_16BIT");
    s_AudioFormat_CHANNEL_OUT_MONO = getIntField(jni, "android/media/AudioFormat", "CHANNEL_OUT_MONO");
    s_AudioFormat_CHANNEL_OUT_STEREO = getIntField(jni, "android/media/AudioFormat", "CHANNEL_OUT_STEREO");
    s_AudioTrack_MODE_STREAM = getIntField(jni, "android/media/AudioTrack", "MODE_STREAM");
    s_AudioTrack_SUCCESS = getIntField(jni, "android/media/AudioTrack", "SUCCESS");
}

void AudioTrackProxy::shutdown()
{
    CK_ASSERT(s_class);

    JniEnv jni;
    jni->DeleteGlobalRef(s_class);
    s_class = NULL;

    s_ctorId = NULL;
    s_releaseId = NULL;
    s_playId = NULL;
    s_stopId = NULL;
    s_pauseId = NULL;
    s_flushId = NULL;
    s_getPlaybackHeadPositionId = NULL;
    s_setPlaybackHeadPositionId = NULL;
    s_setPlaybackRateId = NULL;
    s_setStereoVolumeId = NULL;
    s_writeId = NULL;
    s_getMinBufferSizeId = NULL;
    s_getNativeOutputSampleRateId = NULL;
}


}
