#include "mixer.h"
#include "proxy.h"
#include "stringconvert.h"
#include "ck/core/debug.h"
#include "ck/mixer.h"

namespace CricketTechnology
{
namespace Audio
{

Platform::String^ Mixer::Name::get()
{
    return StringConvert<256>::newPlatformString(m_impl->getName());
}

void Mixer::Name::set(Platform::String^ name)
{
    StringConvert<256> convert(name);
    m_impl->setName(convert.getCString());
}

float Mixer::Volume::get()
{
    return m_impl->getVolume();
}

void Mixer::Volume::set(float volume)
{
    m_impl->setVolume(volume);
}

float Mixer::MixedVolume::get()
{
    return m_impl->getMixedVolume();
}

bool Mixer::Paused::get()
{
    return m_impl->isPaused();
}

void Mixer::Paused::set(bool paused)
{
    m_impl->setPaused(paused);
}

bool Mixer::MixedPauseState::get()
{
    return m_impl->getMixedPauseState();
}

Mixer^ Mixer::Parent::get()
{
    return Proxy::GetMixer(m_impl->getParent());
}

void Mixer::Parent::set(Mixer^ parent)
{
    m_impl->setParent(parent ? parent->Impl : NULL);
}

Mixer^ Mixer::Master::get()
{
    return Proxy::GetMixer(CkMixer::getMaster());
}

Mixer^ Mixer::NewMixer(Platform::String^ name, Mixer^ parent)
{
    StringConvert<256> convert(name);
    CkMixer* impl = CkMixer::newMixer(convert.getCString(), parent ? parent->Impl : NULL);
    return Proxy::GetMixer(impl);
}

Mixer^ Mixer::NewMixer(Platform::String^ name)
{
    return NewMixer(name, nullptr);
}

Mixer^ Mixer::Find(Platform::String^ name)
{
    StringConvert<256> convert(name);
    CkMixer* impl = CkMixer::find(convert.getCString());
    return Proxy::GetMixer(impl);
}

void Mixer::Destroy()
{
    if (m_impl)
    {
        CkMixer* impl = m_impl;
        m_impl = NULL;
        impl->destroy();
    }
}

////////////////////////////////////////

Mixer::Mixer(CkMixer* impl) :
    m_impl(impl)
{}

Mixer::~Mixer()
{
    Destroy();
}

}
}
