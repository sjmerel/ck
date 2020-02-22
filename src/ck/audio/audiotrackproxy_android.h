#pragma once

#include "ck/core/platform.h"
#include "ck/core/allocatable.h"
#include <jni.h>

namespace Cki
{


class AudioTrackProxy : public Allocatable
{
public:
    AudioTrackProxy(int sampleRate, int channels, int bufFrames);
    ~AudioTrackProxy();

    void play();
    void stop();
    void pause();
    void flush();
    int getPlaybackHeadPosition();
    void setPlaybackHeadPosition(int pos);
    void setPlaybackRate(int rate);
    void setStereoVolume(float left, float right);
    int write(jshortArray, int count);
    static int getMinBufferSize(int sampleRate, int channels);
    static int getNativeOutputSampleRate();

    static void init();
    static void shutdown();

private:
    AudioTrackProxy(const AudioTrackProxy&);
    AudioTrackProxy& operator=(const AudioTrackProxy&);

    jobject m_audioTrack;

    // cached class object
    static jclass s_class;

    // cached method ids
    static jmethodID s_ctorId;
    static jmethodID s_releaseId;
    static jmethodID s_playId;
    static jmethodID s_stopId;
    static jmethodID s_pauseId;
    static jmethodID s_flushId;
    static jmethodID s_getPlaybackHeadPositionId;
    static jmethodID s_setPlaybackHeadPositionId;
    static jmethodID s_setPlaybackRateId;
    static jmethodID s_setStereoVolumeId;
    static jmethodID s_writeId;
    static jmethodID s_getMinBufferSizeId;
    static jmethodID s_getNativeOutputSampleRateId;

    // constants
    static int s_AudioManager_STREAM_MUSIC;
    static int s_AudioFormat_ENCODING_PCM_16BIT;
    static int s_AudioFormat_CHANNEL_OUT_MONO;
    static int s_AudioFormat_CHANNEL_OUT_STEREO;
    static int s_AudioTrack_MODE_STREAM;
    static int s_AudioTrack_SUCCESS;
};


}
