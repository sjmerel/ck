#pragma once

#include "ck/core/platform.h"
#include "ck/core/mutex.h"
#include "ck/core/listable.h"
#include "ck/core/deletable.h"
#include "ck/audio/sound.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace Cki
{


class NativeStreamSound : 
    public Sound, 
    public Deletable,
    public Listable<NativeStreamSound>
{
public:
    // URI can be a simple file path (e.g. "/mnt/sdcard/foo.mp3"), or
    // a network resource (e.g. "http://foo.com/bar.mp3").  Playing 
    // network resources requires the android.permissions.INTERNET
    // permission in the manifest.
    // If isAsset is true, uri must be an asset path.
    NativeStreamSound(const char* uri, int offset, int length, bool isAsset = false);

    virtual void destroy();
    virtual bool isReadyToDelete() const;

    virtual ~NativeStreamSound();

    virtual bool isFailed() const;

    virtual bool isPlaying() const;

    virtual void setLoop(int startFrame, int endFrame);
    virtual void getLoop(int& startFrame, int& endFrame) const;
    virtual void setLoopCount(int loopCount);
    virtual int getLoopCount() const;
    virtual int getCurrentLoop() const;
    virtual void releaseLoop();
    virtual bool isLoopReleased() const;

    virtual void setPlayPosition(int);
    virtual void setPlayPositionMs(float);
    virtual int getPlayPosition() const;
    virtual float getPlayPositionMs() const;

    virtual int getLength() const;
    virtual float getLengthMs() const;
    virtual int getSampleRate() const;
    virtual int getChannels() const;

    virtual void setEffectBus(CkEffectBus*);
    virtual EffectBus* getEffectBus();

    void suspend();
    static void suspendAll();

    void resume();
    static void resumeAll();

    virtual void updateVolumeAndPan();

protected:
    virtual void playSub();
    virtual void updateSub() {}
    virtual void stopSub();
    virtual bool isReadySub() const;

    virtual void updatePaused();
    virtual void updateSpeed();
    virtual void updateVirtual();

private:
    SLObjectItf m_playerObj;
    SLPlayItf m_playerPlay;
    SLSeekItf m_playerSeek;
    SLPrefetchStatusItf m_playerPrefetchStatus;
    SLVolumeItf m_playerVolume;
    SLMuteSoloItf m_playerMuteSolo;
    int m_loopCount;
    bool m_ready;
    bool m_playing;
    bool m_suspended;
    int m_fd;
    bool m_fail;
    int m_sampleRate; // -1 if unknown
    int m_currentLoop;
    bool m_releaseLoop;
    SLmillisecond m_prevMs;
    Mutex m_mutex;
    Timer m_destroyTimer;

    void init(const char* uri, int offset, int length, bool isAsset);
    void updatePauseState();

    static void prefetchStatusCallback(SLPrefetchStatusItf, void* context, SLuint32 event);
    void prefetchStatus(SLuint32 event);
    static void playStatusCallback(SLPlayItf, void* context, SLuint32 event);
    void playStatus(SLuint32 event);
};



}

