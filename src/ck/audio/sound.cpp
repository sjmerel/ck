#include "ck/audio/sound.h"
#include "ck/audio/banksound.h"
#include "ck/audio/bank.h"
#include "ck/audio/sample.h"
#include "ck/audio/cksstreamsound.h"
#include "ck/audio/customstreamsound.h"
#include "ck/audio/vorbisstreamsound.h"
#include "ck/audio/mixer.h"
#include "ck/core/string.h"
#include "ck/core/path.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"
#include "ck/core/readstream.h"
#include "ck/core/system.h"

#if CK_PLATFORM_ANDROID
#  include "ck/audio/nativestreamsound_android.h"
#  include "ck/audio/nativestreamsound_android_ics.h"
#  include "ck/core/system_android.h"
#elif CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
#  include "ck/audio/assetstreamsound_ios.h"
#  include "ck/audio/nativestreamsound_ios.h"
#  include "ck/core/system_ios.h"
#endif

// template instantiations
#include "ck/core/proxied.cpp"
#include "ck/core/list.cpp"

namespace Cki
{

////////////////////////////////////////

Sound::AttenParams::AttenParams() 
{
    set(kCkAttenuationMode_InvDistanceSquared, 1.0f, 100.0f, 0.0f);
}

void Sound::AttenParams::set(CkAttenuationMode mode, float nearDist, float farDist, float farVol)
{
    const float k_minDist = 1.0e-12f; // minimum attenuation distance
    m_mode = mode;
    m_nearDist = Math::max(nearDist, k_minDist);
    m_farDist = Math::max(farDist, m_nearDist + k_minDist);
    m_farVol = Math::clamp(farVol, 0.0f, 1.0f);

    switch (mode)
    {
        case kCkAttenuationMode_InvDistance:
            {
                // v = a/r + b
                // a = (vf - 1)/(1/rf - 1/rn)
                // where vf = far volume
                //       rf = far distance
                //       rn = near distance
                float rnInv = 1.0f / m_nearDist;
                float rfInv = 1.0f / m_farDist;
                float a = (m_farVol - 1.0f)/(rfInv - rnInv);
                float b = 1.0f - a*rnInv;
                m_consts[0] = a;
                m_consts[1] = b;
            }
            break;

        case kCkAttenuationMode_InvDistanceSquared:
            {
                // v = a/r^2 + b
                // a = (vf - 1)/(1/rf^2 - 1/rn^2)
                // where vf = far volume
                //       rf = far distance
                //       rn = near distance
                float rnInvSq = 1.0f / (m_nearDist * m_nearDist);
                float rfInvSq = 1.0f / (m_farDist * m_farDist);
                float a = (m_farVol - 1.0f)/(rfInvSq - rnInvSq);
                float b = 1.0f - a*rnInvSq;
                m_consts[0] = a;
                m_consts[1] = b;
            }
            break;

        default:
            m_consts[0] = m_consts[1] = 0.0f;
            break;
    }
}

////////////////////////////////////////

SoundList Sound::s_soundList;

Vector3 Sound::s_3dListenerPos(0.0f, 0.0f, 0.0f);
Vector3 Sound::s_3dListenerVel(0.0f, 0.0f, 0.0f);
Vector3 Sound::s_3dListenerLookAt(0.0f, 0.0f, 1.0f);
Vector3 Sound::s_3dListenerUp(0.0f, 0.1f, 0.0f);
Sound::AttenParams Sound::s_3dAtten;
float Sound::s_3dSoundSpeed = 0.0f;

CkSound::CustomStreamFunc Sound::s_handler = NULL;
void* Sound::s_handlerData = NULL;

Sound::Sound() :
    m_volume(1.0f),
    m_volumeSet(false),
    m_pan(0.0f),
    m_panMatrix(),
    m_panMatrixExplicit(false),
    m_panSet(false),
    m_paused(false),
    m_speed(1.0f),
    m_mixer(Mixer::getMaster()),
    m_nextSound(NULL),
    m_3d(false),
    m_3dVol(1.0f),
    m_3dPan(0.0f),
    m_3dSpeed(1.0f),
    m_3dPos(0.0f, 0.0f, 0.0f),
    m_3dVel(0.0f, 0.0f, 0.0f),
    m_virtual(false),
    m_virtualStopMs(-1.0f),
    m_playWhenReady(false)
{
    m_panMatrix.setZero();
    s_soundList.addFirst(this);
    m_mixer->addSound(this); // master
}

Sound::~Sound()
{
}

void Sound::destroy()
{
    destroyImpl();
    delete this;
}

void Sound::setMixer(CkMixer* mixer)
{
    if (!mixer)
    {
        mixer = Mixer::getMaster();
    }

    if (m_mixer != mixer)
    {
        m_mixer->removeSound(this);
        m_mixer = (Mixer*) mixer;
        m_mixer->addSound(this);
        if (isPlaying() || m_prevSounds.getSize())
        {
            updateVolumeAndPan();
        }
    }
}

CkMixer* Sound::getMixer()
{
    return m_mixer;
}

bool Sound::isReady() const
{
    return isReadySub() && !m_playWhenReady;
}

void Sound::play()
{
    if (isFailed())
    {
        CK_LOG_ERROR("tried to play sound that failed to load");
        return;
    }

    stop();

    if (isReadySub())
    {
        if (m_3d)
        {
            update3d();
        }
        updateVolumeAndPan();
        updateSpeed();

        playSub();

        if (isVirtual())
        {
            startVirtualTimer();
        }

        m_playWhenReady = false;
    }
    else
    {
        CK_LOG_INFO("sound is not ready; will delay until it is");
        m_playWhenReady = true;
    }
}

void Sound::stop()
{
    stopSub();
    m_playWhenReady = false;
}

void Sound::setPaused(bool paused)
{
    if (m_paused != paused)
    {
        m_paused = paused;

        if (getMixedPauseState())
        {
            m_virtualTimer.stop();
        }
        else
        {
            m_virtualTimer.start();
        }

        updatePaused();
    }
}

bool Sound::isPaused() const
{
    return m_paused;
}

bool Sound::getMixedPauseState() const
{
    return m_paused || m_mixer->getMixedPauseState();
}

void Sound::setVolume(float volume)
{
    m_volumeSet = true;
    if (m_volume != volume)
    {
        m_volume = volume;
        if (isPlaying() || m_prevSounds.getSize())
        {
            updateVolumeAndPan();
        }
    }
}

float Sound::getVolume() const
{
    return m_volume;
}

float Sound::getMixedVolume() const
{
    return getVolume() * m_mixer->getMixedVolume();
}

void Sound::setPan(float pan)
{
    m_panSet = true;
    pan = Math::clamp(pan, -1.0f, 1.0f);
    if (m_pan != pan || m_panMatrixExplicit)
    {
        m_panMatrixExplicit = false;
        m_pan = pan;
        if (isPlaying() || m_prevSounds.getSize())
        {
            updateVolumeAndPan();
        }
    }
}

float Sound::getPan() const
{
    if (m_panMatrixExplicit)
    {
        // pan matrix was set explicitly; determine a pan value as best we can from the matrix
        if (getChannels() < 2)
        {
            // assume mono pan if number of channels is not known
            return m_panMatrix.getPan();
        }
        else
        {
            return m_panMatrix.getStereoPan();
        }
    }
    else
    {
        return m_pan;
    }
}

void Sound::setPanMatrix(float ll, float lr, float rl, float rr)
{
    m_panSet = true;
    m_panMatrixExplicit = true;
    m_panMatrix.set(ll, lr, rl, rr);
    if (isPlaying() || m_prevSounds.getSize())
    {
        updateVolumeAndPan();
    }
}

void Sound::getPanMatrix(float& ll, float& lr, float& rl, float& rr) const
{
    if (m_panMatrixExplicit)
    {
        ll = m_panMatrix.ll;
        lr = m_panMatrix.lr;
        rl = m_panMatrix.rl;
        rr = m_panMatrix.rr;
    }
    else
    {
        VolumeMatrix mat;
        if (getChannels() < 2)
        {
            // assume mono pan if number of channels is not known
            mat.setPan(m_pan);
        }
        else
        {
            mat.setStereoPan(m_pan);
        }

        ll = mat.ll;
        lr = mat.lr;
        rl = mat.rl;
        rr = mat.rr;
    }
}

void Sound::setSpeed(float speed)
{
    if (m_speed != speed)
    {
        m_speed = speed;
        if (isPlaying() || m_prevSounds.getSize())
        {
            updateSpeed();
        }
    }
}

float Sound::getSpeed() const
{
    return m_speed;
}

void Sound::setPitchShift(float halfSteps)
{
    // each octave (12 half-steps) is a doubling of the frequency, so
    // assuming equal temperament, 
    //
    // f/f0 = 2^(n/12)
    //
    // where f = new frequency,
    //       f0 = original frequency,
    //       n = number of half steps

    setSpeed(Math::pow(2.0f, halfSteps/12.0f));
}

float Sound::getPitchShift() const
{
    // invert relation in setPitchShift()
    return Math::log(2.0f, getSpeed()) * 12.0f;
}

void Sound::setNextSound(CkSound* nextSound)
{
    if (nextSound && (!getSourceNode() || !((Sound*)nextSound)->getSourceNode()))
    {
        CK_LOG_ERROR("setNextSound() does not work with streams in formats other than .cks or Ogg Vorbis.");
    }

    if (m_nextSound)
    {
        m_nextSound->m_prevSounds.remove(this);
    }
    m_nextSound = (Sound*) nextSound;
    if (m_nextSound)
    {
        m_nextSound->m_prevSounds.addLast(this);
    }
}

CkSound* Sound::getNextSound() const
{
    return (CkSound*) m_nextSound;
}

void Sound::set3dEnabled(bool enabled)
{
    if (m_3d != enabled)
    {
        m_3d = enabled;
        if (enabled)
        {
            if (isPlaying() || m_prevSounds.getSize())
            {
                update3d();
            }
        }

        if (isPlaying() || m_prevSounds.getSize())
        {
            updateVolumeAndPan();
            updateSpeed();
            updateVirtual();
        }
    }
}

bool Sound::is3dEnabled() const
{
    return m_3d;
}

bool Sound::isVirtual() const
{
    return m_3d && m_virtual;
}

void Sound::set3dPosition(float x, float y, float z)
{
    m_3dPos.set(x, y, z);
}

void Sound::get3dPosition(float& x, float& y, float& z) const
{
    x = m_3dPos.x;
    y = m_3dPos.y;
    z = m_3dPos.z;
}

void Sound::set3dVelocity(float vx, float vy, float vz)
{
    m_3dVel.set(vx, vy, vz);
}

void Sound::get3dVelocity(float& vx, float& vy, float& vz) const
{
    vx = m_3dVel.x;
    vy = m_3dVel.y;
    vz = m_3dVel.z;
}

void Sound::set3dListenerPosition(float eyeX, float eyeY, float eyeZ,
                                  float lookAtX, float lookAtY, float lookAtZ,
                                  float upX, float upY, float upZ)
{
    // TODO use matrix instead?
    s_3dListenerPos.set(eyeX, eyeY, eyeZ);
    s_3dListenerLookAt.set(lookAtX, lookAtY, lookAtZ);

    const float k_minMag = 1.0e-15f;
    Vector3 up(upX, upY, upZ);
    if (up.getMagnitude() < k_minMag)
    {
        CK_LOG_ERROR("Up vector is too small; ignoring");
    }
    else
    {
        Vector3::normalize(s_3dListenerUp, up);
    }
}

void Sound::get3dListenerPosition(float& eyeX, float& eyeY, float& eyeZ,
                                  float& lookAtX, float& lookAtY, float& lookAtZ,
                                  float& upX, float& upY, float& upZ)
{
    eyeX = s_3dListenerPos.x;
    eyeY = s_3dListenerPos.y;
    eyeZ = s_3dListenerPos.z;
    lookAtX = s_3dListenerLookAt.x;
    lookAtY = s_3dListenerLookAt.y;
    lookAtZ = s_3dListenerLookAt.z;
    upX = s_3dListenerUp.x;
    upY = s_3dListenerUp.y;
    upZ = s_3dListenerUp.z;
}

void Sound::set3dListenerVelocity(float vx, float vy, float vz)
{
    s_3dListenerVel.set(vx, vy, vz);
}

void Sound::get3dListenerVelocity(float& vx, float& vy, float& vz)
{
    vx = s_3dListenerVel.x;
    vy = s_3dListenerVel.y;
    vz = s_3dListenerVel.z;
}

void Sound::set3dAttenuation(CkAttenuationMode mode, float nearDist, float farDist, float farVol)
{
    s_3dAtten.set(mode, nearDist, farDist, farVol);
}

void Sound::get3dAttenuation(CkAttenuationMode& mode, float& nearDist, float& farDist, float& farVol)
{
    mode = s_3dAtten.m_mode;
    nearDist = s_3dAtten.m_nearDist;
    farDist = s_3dAtten.m_farDist;
    farVol = s_3dAtten.m_farVol;
}

void Sound::set3dSoundSpeed(float speed)
{
    s_3dSoundSpeed = speed;
}

float Sound::get3dSoundSpeed()
{
    return s_3dSoundSpeed;
}

void Sound::updateAll()
{
    Sound* p = s_soundList.getFirst();
    while (p)
    {
        p->update();
        p = ((SoundList::Node*) p)->getNext();
    }
}

SourceNode* Sound::getSourceNode()
{
    return NULL;
}

Sound* Sound::newBankSound(Bank* bank, int index)
{
    if (index < 0 || index >= bank->getNumSounds())
    {
        CK_LOG_ERROR("Sound index %d out of range", index);
        return NULL;
    }
    const Sample& sample = bank->getSample(index);
    return new BankSound(sample, bank);
}

Sound* Sound::newBankSound(Bank* bank, const char* name)
{
    const Sample* sample = NULL;
    if (bank)
    {
        sample = bank->findSample(name);
    }
    else
    {
        sample = Bank::findSample(name, &bank);
    }

    if (sample)
    {
        return new BankSound(*sample, bank);
    }
    else
    {
        CK_LOG_ERROR("Could not find sample \"%s\"", name);
        return NULL;
    }
}

Sound* Sound::newStreamSound(const char* path, CkPathType pathType, int offset, int length, const char* extension)
{
    if (s_handler)
    {
        Path fullPath(path, pathType);
        CkCustomStream* stream = s_handler(fullPath.getBuffer(), s_handlerData);
        if (stream)
        {
            return new CustomStreamSound(stream);
        }
    }

    if (!ReadStream::exists(path, pathType))
    {
        Path fullPath(path, pathType);
        CK_LOG_ERROR("Stream file \"%s\" could not be opened", fullPath.getBuffer());
        return NULL;
    }

    int fileSize = ReadStream::getSize(path, pathType);
    if (offset < 0 || offset >= fileSize)
    {
        CK_LOG_ERROR("Invalid offset %d for stream file \"%s\"", offset, Path(path, pathType).getBuffer());
        return NULL;
    }
    if (length <= 0)
    {
        length = fileSize - offset;
    }
    if (length > fileSize - offset)
    {
        CK_LOG_ERROR("Invalid length %d for stream file \"%s\"", length, Path(path, pathType).getBuffer());
        return NULL;
    }
    if (offset > 0 && !extension)
    {
        CK_LOG_ERROR("File extension must be specified for embedded stream file \"%s\"", Path(path, pathType).getBuffer());
        return NULL;
    }

    Path fullPath(path, pathType);
    if (!extension)
    {
        extension = fullPath.getExtension();
    }

    String extStr((char*) extension, String::External());
    if (extStr.endsWith("cks", true))
    {
        return new CksStreamSound(path, pathType, offset, length);
    }
    else if (extStr.endsWith("ogg") CK_ANDROID_ONLY(&& System::get()->getConfig().enableOggVorbis))
    {
        // TODO plugin
        return new VorbisStreamSound(path, pathType, offset, length);
    }
    else
    {
#if CK_PLATFORM_ANDROID
        SystemAndroid* system = SystemAndroid::get();
        int sdkVersion = system->getSdkVersion();
        /*
        if (sdkVersion >= 14)
        {
            return new NativeStreamSoundIcs(fullPath.getBuffer(), pathType == kCkPathType_Asset);
        }
        else */if (sdkVersion >= 9)
        {
            return new NativeStreamSound(fullPath.getBuffer(), offset, length, pathType == kCkPathType_Asset);
        }
        else
        {
            CK_LOG_ERROR("Only .cks or .ogg streams are currently supported on Android versions prior to 2.3");
            return NULL; // TODO problems with MediaPlayer through JNI
        }
#elif CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
        return new NativeStreamSound(fullPath.getBuffer(), offset, length);
#else
        return NULL;
#endif
    }
}

void Sound::setCustomStreamHandler(CustomStreamFunc func, void* data)
{
    s_handler = func;
    s_handlerData = data;
}

#if CK_PLATFORM_IOS
Sound* Sound::newAssetStreamSound(const char* url)
{
    if (SystemIos::get()->isVersionGreaterThanOrEqualTo("4.3"))
    {
        return new AssetStreamSound(url);
    }
    else
    {
        CK_LOG_ERROR("Asset streams are only supported in iOS 4.3 or later");
        return NULL;
    }
}
#endif

////////////////////////////////////////

void Sound::destroyImpl()
{
    s_soundList.remove(this);

    setNextSound(NULL);
    removePrevSounds();

    stop();
    if (m_mixer)
    {
        m_mixer->removeSound(this);
    }
    Proxied<Sound>::destroy();
}

void Sound::removePrevSounds()
{
    Sound* p = m_prevSounds.getFirst();
    while (p)
    {
        p->setNextSound(NULL);
        p = ((SoundPrevList::Node*) p)->getNext();
    }
}

float Sound::getFinalVolume() const
{
    float vol = getMixedVolume();
    if (m_3d)
    {
        vol *= m_3dVol;
    }
    return vol;
}

float Sound::getFinalPan() const
{
    if (!m_3d && m_panMatrixExplicit)
    {
        if (getChannels() < 2)
        {
            return m_panMatrix.getPan();
        }
        else
        {
            return m_panMatrix.getStereoPan();
        }
    }
    else
    {
        return (m_3d ? m_3dPan : m_pan);
    }
}

void Sound::getFinalVolumeMatrix(VolumeMatrix& mat) const
{
    if (!m_3d && m_panMatrixExplicit)
    {
        mat = m_panMatrix;
    }
    else
    {
        float pan = (m_3d ? m_3dPan : m_pan);
        if (getChannels() < 2)
        {
            mat.setPan(pan);
        }
        else
        {
            mat.setStereoPan(pan);
        }
    }

    mat *= getFinalVolume();
}

float Sound::getFinalSpeed() const
{
    float speed = m_speed;
    if (m_3d)
    {
        speed *= m_3dSpeed;
    }
    return speed;
}

void Sound::update()
{
    updateSub();

    if (m_3d && isPlaying())
    {
        update3d();
    }

    if (m_playWhenReady && isReadySub())
    {
        play();
        m_playWhenReady = false;
    }

    if (isPlaying() && isVirtual())
    {
        // very rough simulation of progress of virtual sound: if it would have stopped by now
        if (m_virtualStopMs >= 0.0f && m_virtualTimer.getElapsedMs() >= m_virtualStopMs)
        {
            stop();
        }
    }
}

void Sound::update3d()
{
    CK_ASSERT(m_3d);

    Vector3 d = m_3dPos - s_3dListenerPos; // vector from listener to object

    // distance attenuation
    //  when dist > far, volume = farVol
    //  when dist < near, volume = 1.0f
    // TODO: sound-specific attenuation
    float dist = d.getMagnitude();
    float invDist = 1.0f / dist;

    float vol = 0.0f;
    if (dist <= s_3dAtten.m_nearDist)
    {
        vol = 1.0f;
    }
    else if (dist >= s_3dAtten.m_farDist)
    {
        vol = s_3dAtten.m_farVol;
    }
    else
    {
        switch (s_3dAtten.m_mode)
        {
            case kCkAttenuationMode_None:
                vol = 1.0f;
                break;

            case kCkAttenuationMode_Linear:
                vol = Math::remap(dist, s_3dAtten.m_nearDist, s_3dAtten.m_farDist, 1.0f, s_3dAtten.m_farVol);
                break;

            case kCkAttenuationMode_InvDistance:
                {
                    // v = a/r + b
                    // a = (vf - 1)/(1/rf - 1/rn)
                    // where vf = far volume
                    //       rf = far distance
                    //       rn = near distance
                    vol = s_3dAtten.m_consts[0] * invDist + s_3dAtten.m_consts[1];
                }
                break;

            case kCkAttenuationMode_InvDistanceSquared:
                {
                    // v = a/r^2 + b
                    // a = (vf - 1)/(1/rf^2 - 1/rn^2)
                    // where vf = far volume
                    //       rf = far distance
                    //       rn = near distance
                    vol = s_3dAtten.m_consts[0] * invDist * invDist + s_3dAtten.m_consts[1];
                }
                break;
        }
        vol = Math::clamp(vol, s_3dAtten.m_farVol, 1.0f);
    }



    // panning
    //   when dist > near, pan based on projection of d on ear plane
    //   when dist < nearMin, pan = 0
    //   when dist is between near and nearMin, linearly interpolate

    float nearMinDist = 0.1f * s_3dAtten.m_nearDist; // TODO configurable
    float pan;
    if (dist < nearMinDist)
    {
        pan = 0.0f;
    }
    else
    {
        Vector3 listenerForward = s_3dListenerLookAt - s_3dListenerPos;

        // project onto listener's ear plane:
        //   up x (d x up) 
        Vector3 dPlane = s_3dListenerUp.cross(d.cross(s_3dListenerUp));
        float dMag = dPlane.getMagnitude();
        if (dMag < 0.0001f)
        {
            // object is nearly right above listener
            pan = 0.0f;
        }
        else
        {
            Vector3::scale(dPlane, dPlane, 1.0f/dMag);

            Vector3 listenerRight = listenerForward.cross(s_3dListenerUp);
            listenerRight.normalize();
            pan = dPlane.dot(listenerRight);
        }
        if (dist < s_3dAtten.m_nearDist)
        {
            pan = Math::remap(dist, nearMinDist, s_3dAtten.m_nearDist, 0.0f, pan);
        }
    }


    // doppler
    if (s_3dSoundSpeed > 0.0f)
    {
        float vMin = -s_3dSoundSpeed * 0.99f;
        float vs = m_3dVel.dot(d) * invDist; // component of source velocity towards listener
        float vr = s_3dListenerVel.dot(-d) * invDist; // component of source velocity towards listener
        vs = Math::max(vs, vMin);
        vr = Math::max(vr, vMin);
        float freqRatio = (s_3dSoundSpeed + vr) / (s_3dSoundSpeed + vs);
        if (freqRatio != m_3dSpeed)
        {
            m_3dSpeed = freqRatio;
            updateSpeed();
        }
    }

    // virtual?
    setVirtual(vol < 0.005f);

    if (vol != m_3dVol || (pan != m_3dPan && vol > 0.001f))
    {
        m_3dVol = vol;
        m_3dPan = pan;
        updateVolumeAndPan();
    }
}

void Sound::setVirtual(bool virt)
{
    if (virt != m_virtual)
    {
        m_virtual = virt;
        updateVirtual();

        if (virt)
        {
            startVirtualTimer();
        }
    }
}

void Sound::startVirtualTimer()
{
    m_virtualStopMs = getMsLeft();
    m_virtualTimer.reset();
    if (!getMixedPauseState())
    {
        m_virtualTimer.start();
    }
}

float Sound::getMsLeft()
{
    // very rough calculation of when to stop a virtual voice. 
    int loopCount = getLoopCount();
    if (loopCount == 0 || isLoopReleased())
    {
        // no looping
        return (getLengthMs() - getPlayPositionMs()) / getSpeed();
    }
    else if (loopCount < 0)
    {
        // infinite looping
        return -1.0f;
    }
    else
    {
        int loopStart, loopEnd;
        getLoop(loopStart, loopEnd);
        int sampleRate = getSampleRate();
        float loopStartMs = (float) loopStart / sampleRate;
        float loopEndMs = (float) loopEnd / sampleRate;
        int loopsLeft = loopCount - getCurrentLoop();
        return (loopsLeft * (loopEndMs - loopStartMs) + (getLengthMs() - getPlayPositionMs())) / getSpeed();
    }
}



template class Proxied<Sound>;
template class List<Sound, 0>;
template class List<Sound, 1>;
template class List<Sound, 2>;

}
