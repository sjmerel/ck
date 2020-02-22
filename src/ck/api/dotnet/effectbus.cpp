#include "effectbus.h"
#include "effect.h"
#include "proxy.h"
#include "ck/core/debug.h"
#include "ck/effectbus.h"
#include "ck/effect.h"

namespace CricketTechnology
{
namespace Audio
{

void EffectBus::AddEffect(Effect^ effect)
{
    m_impl->addEffect(effect->Impl);
}

void EffectBus::RemoveEffect(Effect^ effect)
{
    m_impl->removeEffect(effect->Impl);
}

void EffectBus::RemoveAllEffects()
{
    m_impl->removeAllEffects();
}

EffectBus^ EffectBus::OutputBus::get()
{
    return Proxy::GetEffectBus(m_impl->getOutputBus());
}

void EffectBus::OutputBus::set(EffectBus^ bus)
{
    m_impl->setOutputBus(bus ? bus->Impl : NULL);
}

void EffectBus::Reset()
{
    m_impl->reset();
}

bool EffectBus::Bypassed::get()
{
    return m_impl->isBypassed();
}

void EffectBus::Bypassed::set(bool bypassed)
{
    m_impl->setBypassed(bypassed);
}

float EffectBus::WetDryRatio::get()
{
    return m_impl->getWetDryRatio();
}

void EffectBus::WetDryRatio::set(float ratio)
{
    m_impl->setWetDryRatio(ratio);
}

EffectBus^ EffectBus::NewEffectBus()
{
    return Proxy::GetEffectBus(CkEffectBus::newEffectBus());
}

EffectBus^ EffectBus::GlobalEffectBus::get()
{
    return Proxy::GetEffectBus(CkEffectBus::getGlobalEffectBus());
}

void EffectBus::Destroy()
{
    if (m_impl)
    {
        m_impl->destroy();
        m_impl = NULL;
    }
}

EffectBus::EffectBus(CkEffectBus* impl) :
    m_impl(impl)
{}

EffectBus::~EffectBus()
{
    Destroy();
}

}
}
