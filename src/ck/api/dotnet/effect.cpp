#include "effect.h"
#include "proxy.h"
#include "ck/core/debug.h"
#include "ck/effect.h"

namespace CricketTechnology
{
namespace Audio
{

void Effect::SetParam(int paramId, float value)
{
    m_impl->setParam(paramId, value);
}

void Effect::Reset()
{
    m_impl->reset();
}

bool Effect::Bypassed::get()
{
    return m_impl->isBypassed();
}

void Effect::Bypassed::set(bool bypassed)
{
    m_impl->setBypassed(bypassed);
}

float Effect::WetDryRatio::get()
{
    return m_impl->getWetDryRatio();
}

void Effect::WetDryRatio::set(float ratio)
{
    m_impl->setWetDryRatio(ratio);
}

Effect^ Effect::NewEffect(EffectType type)
{
    CkEffect* impl = CkEffect::newEffect((CkEffectType) type);
    return Proxy::GetEffect(impl);
}

Effect^ Effect::NewCustomEffect(int id)
{
    CkEffect* impl = CkEffect::newCustomEffect(id);
    return Proxy::GetEffect(impl);
}

void Effect::Destroy()
{
    if (m_impl)
    {
        CkEffect* impl = m_impl;
        m_impl = NULL;
        impl->destroy();
    }
}

Effect::Effect(CkEffect* impl) :
    m_impl(impl)
{}

Effect::~Effect()
{
    Destroy();
}

}
}
