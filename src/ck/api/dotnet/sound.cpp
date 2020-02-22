#include "sound.h"
#include "mixer.h"
#include "effectbus.h"
#include "bank.h"
#include "proxy.h"
#include "stringconvert.h"
#include "ck/core/debug.h"
#include "ck/sound.h"
#include "ck/mixer.h"

namespace CricketTechnology
{
namespace Audio
{

Mixer^ Sound::Mixer::get()
{
    return Proxy::GetMixer(m_impl->getMixer());
}

void Sound::Mixer::set(CricketTechnology::Audio::Mixer^ mixer)
{
    m_impl->setMixer(mixer ? mixer->Impl : NULL);
}

bool Sound::Ready::get()
{
    return m_impl->isReady();
}

bool Sound::Failed::get()
{
    return m_impl->isFailed();
}

void Sound::Play()
{
    m_impl->play();
}

void Sound::Stop()
{
    m_impl->stop();
}

bool Sound::Playing::get()
{
    return m_impl->isPlaying();
}

bool Sound::Paused::get()
{
    return m_impl->isPaused();
}

void Sound::Paused::set(bool paused)
{
    m_impl->setPaused(paused);
}

bool Sound::MixedPauseState::get()
{
    return m_impl->getMixedPauseState();
}

void Sound::SetLoop(int startFrame, int endFrame)
{
    m_impl->setLoop(startFrame, endFrame);
}

void Sound::GetLoop(int* startFrame, int* endFrame)
{
    m_impl->getLoop(*startFrame, *endFrame);
}

int Sound::LoopCount::get()
{
    return m_impl->getLoopCount();
}

void Sound::LoopCount::set(int loopCount)
{
    m_impl->setLoopCount(loopCount);
}

int Sound::CurrentLoop::get()
{
    return m_impl->getCurrentLoop();
}

void Sound::ReleaseLoop()
{
    m_impl->releaseLoop();
}

bool Sound::LoopReleased::get()
{
    return m_impl->isLoopReleased();
}

int Sound::PlayPosition::get()
{
    return m_impl->getPlayPosition();
}

void Sound::PlayPosition::set(int pos)
{
    m_impl->setPlayPosition(pos);
}

float Sound::PlayPositionMs::get()
{
    return m_impl->getPlayPositionMs();
}

void Sound::PlayPositionMs::set(float ms)
{
    m_impl->setPlayPositionMs(ms);
}

float Sound::Volume::get()
{
    return m_impl->getVolume();
}

void Sound::Volume::set(float vol)
{
    m_impl->setVolume(vol);
}

float Sound::MixedVolume::get()
{
    return m_impl->getMixedVolume();
}

float Sound::Pan::get()
{
    return m_impl->getPan();
}

void Sound::Pan::set(float pan)
{
    m_impl->setPan(pan);
}

void Sound::SetPanMatrix(float ll, float lr, float rl, float rr)
{
    m_impl->setPanMatrix(ll, lr, rl, rr);
}

void Sound::GetPanMatrix(float* ll, float* lr, float* rl, float* rr)
{
    m_impl->getPanMatrix(*ll, *lr, *rl, *rr);
}

float Sound::PitchShift::get()
{
    return m_impl->getPitchShift();
}

void Sound::PitchShift::set(float p)
{
    m_impl->setPitchShift(p);
}

float Sound::Speed::get()
{
    return m_impl->getSpeed();
}

Sound^ Sound::NextSound::get()
{
    return Proxy::GetSound(m_impl->getNextSound());
}

void Sound::NextSound::set(CricketTechnology::Audio::Sound^ next)
{
    m_impl->setNextSound(next ? next->Impl : NULL);
}

void Sound::Speed::set(float speed)
{
    m_impl->setSpeed(speed);
}

int Sound::Length::get()
{
    return m_impl->getLength();
}

float Sound::LengthMs::get()
{
    return m_impl->getLengthMs();
}

int Sound::SampleRate::get()
{
    return m_impl->getSampleRate();
}

int Sound::Channels::get()
{
    return m_impl->getChannels();
}

EffectBus^ Sound::EffectBus::get()
{
    return Proxy::GetEffectBus(m_impl->getEffectBus());
}

void Sound::EffectBus::set(CricketTechnology::Audio::EffectBus^ b)
{
    m_impl->setEffectBus(b ? b->Impl : NULL);
}

bool Sound::ThreeDEnabled::get()
{
    return m_impl->is3dEnabled();
}

void Sound::ThreeDEnabled::set(bool enabled)
{
    m_impl->set3dEnabled(enabled);
}

bool Sound::Virtual::get()
{
    return m_impl->isVirtual();
}

void Sound::Set3dPosition(float x, float y, float z)
{
    m_impl->set3dPosition(x, y, z);
}

void Sound::Get3dPosition(float* x, float* y, float* z)
{
    m_impl->get3dPosition(*x, *y, *z);
}

void Sound::Set3dVelocity(float vx, float vy, float vz)
{
    m_impl->set3dVelocity(vx, vy, vz);
}

void Sound::Get3dVelocity(float* vx, float* vy, float* vz)
{
    m_impl->get3dVelocity(*vx, *vy, *vz);
}

void Sound::Set3dListenerPosition(float eyeX, float eyeY, float eyeZ,
        float lookAtX, float lookAtY, float lookAtZ,
        float upX, float upY, float upZ)
{
    CkSound::set3dListenerPosition(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);
}

void Sound::Get3dListenerPosition(float* eyeX, float* eyeY, float* eyeZ,
        float* lookAtX, float* lookAtY, float* lookAtZ,
        float* upX, float* upY, float* upZ)
{
    CkSound::get3dListenerPosition(*eyeX, *eyeY, *eyeZ, *lookAtX, *lookAtY, *lookAtZ, *upX, *upY, *upZ);
}

void Sound::Set3dListenerVelocity(float vx, float vy, float vz)
{
    CkSound::set3dListenerVelocity(vx, vy, vz);
}

void Sound::Get3dListenerVelocity(float* vx, float* vy, float* vz)
{
    CkSound::get3dListenerVelocity(*vx, *vy, *vz);
}

void Sound::Set3dAttenuation(AttenuationMode mode, float nearDist, float farDist, float farVol)
{
    CkSound::set3dAttenuation((CkAttenuationMode) mode, nearDist, farDist, farVol);
}

void Sound::Get3dAttenuation(AttenuationMode* mode, float* nearDist, float* farDist, float* farVol)
{
    CkAttenuationMode m;
    CkSound::get3dAttenuation(m, *nearDist, *farDist, *farVol);
    *mode = (AttenuationMode) m;
}

float Sound::ThreeDSoundSpeed::get()
{
    return CkSound::get3dSoundSpeed();
}

void Sound::ThreeDSoundSpeed::set(float speed)
{
    CkSound::set3dSoundSpeed(speed);
}

float Sound::SoundSpeed_CentimetersPerSecond::get()
{
    return CkSound::k_soundSpeed_CentimetersPerSecond;
}

float Sound::SoundSpeed_MetersPerSecond::get()
{
    return CkSound::k_soundSpeed_MetersPerSecond;
}

float Sound::SoundSpeed_InchesPerSecond::get()
{
    return CkSound::k_soundSpeed_InchesPerSecond;
}

float Sound::SoundSpeed_FeetPerSecond::get()
{
    return CkSound::k_soundSpeed_FeetPerSecond;
}

Sound^ Sound::NewBankSound(Bank^ bank, int index)
{
    CkSound* impl = CkSound::newBankSound(bank->Impl, index);
    return Proxy::GetSound(impl);
}

Sound^ Sound::NewBankSound(Bank^ bank, Platform::String^ name)
{
    StringConvert<256> convert(name);
    CkSound* impl = CkSound::newBankSound(bank ? bank->Impl : NULL, convert.getCString());
    return Proxy::GetSound(impl);
}

Sound^ Sound::NewStreamSound(Platform::String^ path)
{
    return NewStreamSound(path, PathType::Default);
}

Sound^ Sound::NewStreamSound(Platform::String^ path, PathType pathType)
{
    StringConvert<256> convert(path);
    CkSound* impl = CkSound::newStreamSound(convert.getCString(), (CkPathType) pathType, 0, 0, NULL);
    return Proxy::GetSound(impl);
}

Sound^ Sound::NewStreamSound(Platform::String^ path, PathType pathType, int offset, int length, Platform::String^ extension)
{
    StringConvert<256> pathConvert(path);
    StringConvert<256> extensionConvert(extension);
    CkSound* impl = CkSound::newStreamSound(pathConvert.getCString(), (CkPathType) pathType, offset, length, extensionConvert.getCString());
    return Proxy::GetSound(impl);
}


void Sound::Destroy()
{
    if (m_impl)
    {
        CkSound* impl = m_impl;
        m_impl = NULL;
        impl->destroy();
    }
}

Sound::Sound(CkSound* impl) :
    m_impl(impl)
{}

Sound::~Sound()
{
    Destroy();
}

}
}
