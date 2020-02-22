#include "ck/audio/nativestreamsound_android.h"
#include "ck/audio/audiodebug_android.h"
#include "ck/audio/audio_android.h"
#include "ck/audio/opensles_android.h"
#include "ck/audio/audiofileinfo.h"
#include "ck/audio/audiograph.h"
#include "ck/core/system_android.h"
#include "ck/core/path.h"
#include "ck/core/math.h"
#include "ck/core/logger.h"
#include "ck/core/assetmanager_android.h"
#include "ck/core/mutexlock.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

// template definitions
#include "ck/core/listable.cpp"


namespace Cki
{


NativeStreamSound::NativeStreamSound(const char* uri, int offset, int length, bool isAsset) :
    m_playerObj(NULL),
    m_playerPlay(NULL),
    m_playerSeek(NULL),
    m_playerPrefetchStatus(NULL),
    m_playerVolume(NULL),
    m_loopCount(0),
    m_ready(false),
    m_playing(false),
    m_suspended(false),
    m_fd(-1),
    m_fail(false),
    m_sampleRate(-1),
    m_currentLoop(0),
    m_releaseLoop(false),
    m_prevMs(0)
{
    // could do this on separate thread... but seems fast enough (about 0.15 ms)
    AudioFileInfo info(uri, isAsset ? kCkPathType_Asset : kCkPathType_FileSystem);
    if (info.isValid())
    {
        m_sampleRate = info.getSampleRate();
    }

    init(uri, offset, length, isAsset);
}

void NativeStreamSound::destroy()
{
    destroyImpl();
    AudioGraph::get()->deleteObject(this);
    m_destroyTimer.start();
}

bool NativeStreamSound::isReadyToDelete() const
{
    // This is a workaround for an OpenSL ES bug, resulting in a hang and a 
    // warning like this:
    //   W/libOpenSLES( 3899): frameworks/wilhelm/src/android/AudioPlayer_to_android.cpp:779: pthread 0x6faf0c60 (tid 5855) sees object 0x6e9626d8 was locked by pthread 0x40060440 (tid 3915) at frameworks/wilhelm/src/itf/IObject.c:411
    // Workaround is to wait until ready or failed before deleting.
    return (m_ready || m_fail) && (m_destroyTimer.getElapsedMs() > 100.0f);
}

NativeStreamSound::~NativeStreamSound()
{
    if (m_playerObj)
    {
        (*m_playerObj)->Destroy(m_playerObj);
        if (m_fd >= 0)
        {
            close(m_fd);
        }
    }
}

bool NativeStreamSound::isFailed() const
{
    return m_fail;
}

bool NativeStreamSound::isPlaying() const
{
    return m_playing;
}

void NativeStreamSound::setLoop(int startFrame, int endFrame)
{
    if (!(startFrame == 0 && endFrame < 0))
    {
        CK_LOG_ERROR("Native streams can only loop over the entire stream");
    }
}

void NativeStreamSound::getLoop(int& startFrame, int& endFrame) const
{
    startFrame = 0;
    endFrame = -1;
}

void NativeStreamSound::setLoopCount(int loopCount)
{
    MutexLock lock(m_mutex);

    if (m_loopCount != loopCount)
    {
        m_loopCount = loopCount;
        CK_SL_VERIFY( (*m_playerSeek)->SetLoop(m_playerSeek, m_loopCount != 0, 0, SL_TIME_UNKNOWN) );
    }
}

int NativeStreamSound::getLoopCount() const
{
    return m_loopCount;
}

int NativeStreamSound::getCurrentLoop() const
{
    return m_currentLoop;
}

void NativeStreamSound::releaseLoop()
{
    MutexLock lock(m_mutex);

    CK_SL_VERIFY( (*m_playerSeek)->SetLoop(m_playerSeek, false, 0, SL_TIME_UNKNOWN) );
    m_releaseLoop = true;
}

bool NativeStreamSound::isLoopReleased() const
{
    return m_releaseLoop;
}

void NativeStreamSound::setPlayPosition(int frame)
{
    if (m_sampleRate < 0)
    {
        CK_LOG_ERROR("Can't set play position in frames on stream, because sample rate could not be determined; try setPlayPositionMs() instead.");
    }
    else
    {
        setPlayPositionMs(1000.0f * frame / m_sampleRate);
    }
}

void NativeStreamSound::setPlayPositionMs(float ms)
{
    SLmillisecond pos = Math::round(ms);
    CK_SL_VERIFY( (*m_playerSeek)->SetPosition(m_playerSeek, pos, SL_SEEKMODE_ACCURATE) );
}

int NativeStreamSound::getPlayPosition() const 
{ 
    if (m_sampleRate < 0)
    {
        CK_LOG_ERROR("Can't get play position in frames on stream, because sample rate could not be determined; try getPlayPositionMs() instead.");
        return -1; 
    }
    else
    {
        return Math::round(getPlayPositionMs() * 0.001f * m_sampleRate);
    }
} 

float NativeStreamSound::getPlayPositionMs() const 
{ 
    SLmillisecond ms;
    CK_SL_VERIFY( (*m_playerPlay)->GetPosition(m_playerPlay, &ms) );
    return (float) ms;
} 

int NativeStreamSound::getLength() const
{
    if (m_sampleRate < 0)
    {
        CK_LOG_ERROR("Can't get length in frames on stream, because sample rate could not be determined; try getLengthMs() instead.");
        return -1;
    }
    else
    {
        float ms = getLengthMs();
        if (ms < 0.0f)
        {
            return -1;
        }
        else
        {
            return Math::round(ms * 0.001f * m_sampleRate);
        }
    }
}

float NativeStreamSound::getLengthMs() const
{
    if (isReady())
    {
        SLmillisecond msec;
        CK_SL_VERIFY( (*m_playerPlay)->GetDuration(m_playerPlay, &msec) );
        return (float) msec;
    }
    else
    {
        return -1.0f;
    }
}

int NativeStreamSound::getSampleRate() const
{
    if (m_sampleRate < 0)
    {
        CK_LOG_ERROR("Can't determine sample rate of stream");
        return -1;
    }
    else
    {
        return m_sampleRate;
    }
}

int NativeStreamSound::getChannels() const
{
    if (isReady())
    {
        SLuint8 chans;
        CK_SL_VERIFY( (*m_playerMuteSolo)->GetNumChannels(m_playerMuteSolo, &chans) );
        return chans;
    }
    else
    {
        return -1;
    }
}

void NativeStreamSound::setEffectBus(CkEffectBus* bus)
{
    if (bus)
    {
        CK_LOG_ERROR("Effects not supported on native streams");
    }
}

EffectBus* NativeStreamSound::getEffectBus()
{
    return NULL;
}

void NativeStreamSound::suspend()
{
    m_suspended = true;
    updatePauseState();
}

void NativeStreamSound::suspendAll()
{
    NativeStreamSound* p = Listable<NativeStreamSound>::getFirst();
    while (p)
    {
        p->suspend();
        p = ((Listable<NativeStreamSound>*) p)->getNext();
    }
}

void NativeStreamSound::resume()
{
    m_suspended = false;
    updatePauseState();
}

void NativeStreamSound::resumeAll()
{
    NativeStreamSound* p = Listable<NativeStreamSound>::getFirst();
    while (p)
    {
        p->resume();
        p = ((Listable<NativeStreamSound>*) p)->getNext();
    }
}

void NativeStreamSound::updateVolumeAndPan()
{
    MutexLock lock(m_mutex);

    if (m_playerVolume)
    {
        // dB = 20 log10(P1/P0)
        float volume = getFinalVolume();

        // compensate for extra 3dB on stereo streams
        if (getChannels() == 2)
        {
            volume *= 0.5f;
        }

        SLmillibel level = SL_MILLIBEL_MIN;
        if (volume > 0.001f)
        {
            float mB = 2000.0f * Math::log10(volume);
            level = (SLmillibel) Math::max(mB, (float) SL_MILLIBEL_MIN);
        }
        CK_SL_VERIFY( (*m_playerVolume)->SetVolumeLevel(m_playerVolume, level) );

        float pan = getFinalPan();
        SLpermille panPerMille = Math::clamp(Math::round(pan * 1000), -1000, 1000);
        CK_SL_VERIFY( (*m_playerVolume)->SetStereoPosition(m_playerVolume, panPerMille) );
    }
}

////////////////////////////////////////

void NativeStreamSound::playSub()
{
    MutexLock lock(m_mutex);

    // update looping state, in case we released the loop
    CK_SL_VERIFY( (*m_playerSeek)->SetLoop(m_playerSeek, (m_loopCount != 0), 0, SL_TIME_UNKNOWN) );
    m_releaseLoop = false;

    m_playing = true;
    updatePauseState();
}

void NativeStreamSound::stopSub()
{
    MutexLock lock(m_mutex);

    if (m_playing)
    {
        CK_SL_VERIFY( (*m_playerPlay)->SetPlayState(m_playerPlay, SL_PLAYSTATE_STOPPED) );
        m_playing = false;
    }
    m_currentLoop = 0;
    m_prevMs = 0;
}

bool NativeStreamSound::isReadySub() const
{
    return m_ready;
}

void NativeStreamSound::updatePaused()
{
    MutexLock lock(m_mutex);
    updatePauseState();
}

void NativeStreamSound::updateSpeed()
{
    if (getFinalSpeed() != 1.0f)
    {
        CK_LOG_ERROR("Can't set speed on native streams");
    }
}

void NativeStreamSound::updateVirtual()
{
    updatePauseState();
}

////////////////////////////////////////

void NativeStreamSound::init(const char* uri, int offset, int length, bool isAsset)
{
    // source data locator
    SLDataLocator_AndroidFD fdLocator = { 0 };
    fdLocator.locatorType = SL_DATALOCATOR_ANDROIDFD;

    if (isAsset)
    {
        // open asset as file descriptor
        byte assetBuf[AssetManager::k_assetSize];
        AssetManager::AAsset* asset = AssetManager::open(uri, AssetManager::AASSET_MODE_STREAMING, assetBuf);
        if (!asset)
        {
            m_fail = true;
            return;
        }
        off_t assetStart, assetLength;
        m_fd = AssetManager::openFileDescriptor(asset, &assetStart, &assetLength);
        // TODO: do we need to close this fd?  close() will fail on it.
        AssetManager::close(asset);

        if (m_fd < 0)
        {
            m_fail = true;
            return;
        }

        fdLocator.fd = m_fd;
        fdLocator.offset = assetStart + offset;
        fdLocator.length = length;
    }
    else
    {
        m_fd = open(uri, O_RDONLY);
        fdLocator.fd = m_fd;
        fdLocator.offset = offset;
        fdLocator.length = length;
    }

    SLDataFormat_MIME mimeFormat = { 0 };
    mimeFormat.formatType = SL_DATAFORMAT_MIME;
    mimeFormat.mimeType = NULL;
    mimeFormat.containerType = SL_CONTAINERTYPE_UNSPECIFIED;

    // audio source
    SLDataSource audioSrc = { 0 };
    audioSrc.pLocator = &fdLocator;
    audioSrc.pFormat = &mimeFormat;

    // sink data locator
    SLDataLocator_OutputMix outLocator = { 0 };
    outLocator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    outLocator.outputMix = Audio::g_outputMixObj;

    // audio sink
    SLDataSink audioSnk = { 0 };
    audioSnk.pLocator = &outLocator;
    audioSnk.pFormat = NULL;

    // create audio player
    // TODO use voice management
    const SLInterfaceID ids[] = 
    { 
        OpenSLES::SL_IID_SEEK, 
        OpenSLES::SL_IID_PREFETCHSTATUS, 
        OpenSLES::SL_IID_VOLUME, 
        OpenSLES::SL_IID_MUTESOLO 
    };
    const SLboolean req[] = 
    { 
        SL_BOOLEAN_TRUE,       
        SL_BOOLEAN_TRUE,                 
        SL_BOOLEAN_TRUE,         
        SL_BOOLEAN_TRUE 
    };
    CK_SL_VERIFY( (*Audio::g_engine)->CreateAudioPlayer(Audio::g_engine, &m_playerObj, &audioSrc, &audioSnk, CK_ARRAY_COUNT(ids), ids, req) );
    // TODO realize asynchronously?
    CK_SL_VERIFY( (*m_playerObj)->Realize(m_playerObj, SL_BOOLEAN_FALSE) );

    // get play interface
    CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_PLAY, &m_playerPlay) );

    // get seek interface
    CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_SEEK, &m_playerSeek) );

    // get prefetch status interface
    CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_PREFETCHSTATUS, &m_playerPrefetchStatus) );

    // get mute/solo interface
    CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_MUTESOLO, &m_playerMuteSolo) );

    // get volume interface
    CK_SL_VERIFY( (*m_playerObj)->GetInterface(m_playerObj, OpenSLES::SL_IID_VOLUME, &m_playerVolume) );
    CK_SL_VERIFY( (*m_playerVolume)->EnableStereoPosition(m_playerVolume, SL_BOOLEAN_TRUE) );

    // register callback to be notified when we're ready to play and to detect errors
    CK_SL_VERIFY( (*m_playerPrefetchStatus)->SetCallbackEventsMask(m_playerPrefetchStatus, SL_PREFETCHEVENT_STATUSCHANGE | SL_PREFETCHEVENT_FILLLEVELCHANGE) );
    CK_SL_VERIFY( (*m_playerPrefetchStatus)->RegisterCallback(m_playerPrefetchStatus, prefetchStatusCallback, this) );

    // register callback to be notified when playback finishes
    CK_SL_VERIFY( (*m_playerPlay)->RegisterCallback(m_playerPlay, playStatusCallback, this) );
    CK_SL_VERIFY( (*m_playerPlay)->SetCallbackEventsMask(m_playerPlay, SL_PLAYEVENT_HEADATEND | SL_PLAYEVENT_HEADATNEWPOS) );
    CK_SL_VERIFY( (*m_playerPlay)->SetPositionUpdatePeriod(m_playerPlay, 100) );

    // start paused, so prefetch begins
    CK_SL_VERIFY( (*m_playerPlay)->SetPlayState(m_playerPlay, SL_PLAYSTATE_PAUSED) );
}

void NativeStreamSound::updatePauseState()
{
    if (m_playing)
    {
        if (getMixedPauseState() || m_suspended || isVirtual())
        {
            CK_SL_VERIFY( (*m_playerPlay)->SetPlayState(m_playerPlay, SL_PLAYSTATE_PAUSED) );
        }
        else
        {
            CK_SL_VERIFY( (*m_playerPlay)->SetPlayState(m_playerPlay, SL_PLAYSTATE_PLAYING) );
        }
    }
}

void NativeStreamSound::prefetchStatusCallback(SLPrefetchStatusItf, void* context, SLuint32 event)
{
    NativeStreamSound* sound = (NativeStreamSound*) context;
    sound->prefetchStatus(event);
}

void NativeStreamSound::prefetchStatus(SLuint32 event)
{
    if (event & SL_PREFETCHEVENT_STATUSCHANGE)
    {
        SLuint32 status;
        CK_SL_VERIFY( (*m_playerPrefetchStatus)->GetPrefetchStatus(m_playerPrefetchStatus, &status) );

        MutexLock lock(m_mutex);
        if (event & SL_PREFETCHEVENT_FILLLEVELCHANGE)
        {
            // this is the weird way of notifying us of errors, specified in the OpenSL for Android docs
            SLpermille level;
            CK_SL_VERIFY( (*m_playerPrefetchStatus)->GetFillLevel(m_playerPrefetchStatus, &level) );
            if (level == 0 && status == SL_PREFETCHSTATUS_UNDERFLOW)
            {
                m_fail = true;
            }
        }

        if (status == SL_PREFETCHSTATUS_SUFFICIENTDATA)
        {
            m_ready = true;
        }
    }
}

void NativeStreamSound::playStatusCallback(SLPlayItf, void* context, SLuint32 event)
{
    NativeStreamSound* sound = (NativeStreamSound*) context;
    sound->playStatus(event);
}

void NativeStreamSound::playStatus(SLuint32 event)
{
    MutexLock lock(m_mutex);

    if (event & SL_PLAYEVENT_HEADATNEWPOS)
    {
        // NOTE: Android 4.0 doesn't get SL_PLAYEVENT_HEADATEND for looping sounds, so do this:
        SLmillisecond ms;
        CK_SL_VERIFY( (*m_playerPlay)->GetPosition(m_playerPlay, &ms) );

        if (ms < m_prevMs)
        {
            ++m_currentLoop;
            if (m_loopCount == m_currentLoop)
            {
                CK_SL_VERIFY( (*m_playerSeek)->SetLoop(m_playerSeek, false, 0, SL_TIME_UNKNOWN) );
            }
        }
        m_prevMs = ms;
    }

    if (event & SL_PLAYEVENT_HEADATEND)
    {
        SLboolean looping;
        SLmillisecond start, end;
        CK_SL_VERIFY( (*m_playerSeek)->GetLoop(m_playerSeek, &looping, &start, &end) );
        if (!looping)
        {
            stopSub();
        }
    }
}

}
