#include "ck/audio/effect.h"
#include "ck/audio/effectbusnode.h"
#include "ck/audio/audiograph.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"
#include "ck/effectprocessor.h"
#include "ck/audio/bitcrusherprocessor.h"
#include "ck/audio/ringmodprocessor.h"
#include "ck/audio/biquadfilterprocessor.h"
#include "ck/audio/distortionprocessor.h"

// template instantiations
#include "ck/core/proxied.cpp"
#include "ck/core/list.cpp"
#include "ck/core/array.cpp"

namespace Cki
{


Effect::Effect(CkEffectProcessor* proc) :
    m_proc(proc),
    m_bypass(false),
    m_wetDry(1.0f),
    m_owner(NULL)
{
}

Effect::~Effect()
{
    delete m_proc;
}

void Effect::execute(int commandId, CommandParam param0, CommandParam param1)
{
    switch (commandId)
    {
        case k_setParam:
            m_proc->setParam(param0.intValue, param1.floatValue);
            break;

        case k_reset:
            m_proc->reset();
            break;

        default:
            CK_FAIL("unknown command");
    };
}

void Effect::setParam(int paramId, float value)
{
    Cki::AudioGraph::get()->execute(this, k_setParam, paramId, value);
}

void Effect::reset()
{
    Cki::AudioGraph::get()->execute(this, k_reset);
}

void Effect::setBypassed(bool bypass)
{
    if (bypass && !m_bypass)
    {
        reset();
    }
    m_bypass = bypass;
}

bool Effect::isBypassed() const
{
    return m_bypass;
}

void Effect::setWetDryRatio(float wetDry)
{
    m_wetDry = Math::clamp(wetDry, 0.0f, 1.0f);
}

float Effect::getWetDryRatio() const
{
    return m_wetDry;
}

Effect* Effect::newEffect(CkEffectType type)
{
    switch (type)
    {
        case kCkEffectType_BitCrusher:
            return new Effect(new BitCrusherProcessor);

        case kCkEffectType_RingMod:
            return new Effect(new RingModProcessor);

        case kCkEffectType_BiquadFilter:
            return new Effect(new BiquadFilterProcessor);

        case kCkEffectType_Distortion:
            return new Effect(new DistortionProcessor);

        default:
            CK_LOG_ERROR("Unknown effect type %d", type);
            return NULL;
    }
}

void Effect::registerCustomEffect(int id, CustomEffectFunc func)
{
    int index = -1;
    for (int i = 0; i < s_factories.getSize(); ++i)
    {
        if (s_factories[i].id == id)
        {
            index = i;
            break;
        }
    }

    if (func)
    {
        // register
        FactoryInfo* factory = NULL;
        if (index >= 0)
        {
            factory = &s_factories[index];
            CK_LOG_INFO("Replacing effect factory function with id %d", id);
        }
        else
        {
            s_factories.append(FactoryInfo());
            factory = &s_factories.last();
        }
        factory->id = id;
        factory->func = func;
    }
    else
    {
        // unregister
        if (index >= 0)
        {
            s_factories.erase(index);
        }
        else
        {
            CK_LOG_WARNING("Could not unregister effect factory function; none registered with id %d", id);
        }
    }
}

Effect* Effect::newCustomEffect(int id, void* arg)
{
    for (int i = 0; i < s_factories.getSize(); ++i)
    {
        const FactoryInfo& factory = s_factories[i];
        if (factory.id == id)
        {
            CkEffectProcessor* proc = factory.func(arg);
            if (proc)
            {
                return new Effect(proc);
            }
            else
            {
                CK_LOG_ERROR("Effect factory function for id %d returned NULL", id);
                return NULL;
            }
        }
    }

    CK_LOG_ERROR("Effect factory function not registered for id %d", id);
    return NULL;
}

void Effect::setOwner(EffectBusNode* owner)
{
    m_owner = owner;
}

void Effect::destroy()
{
    AudioGraph* graph = AudioGraph::get();

    if (m_owner)
    {
        graph->execute(m_owner, EffectBusNode::k_removeEffect, this);
    }

    graph->deleteObject(this);
}

Array<Effect::FactoryInfo> Effect::s_factories;

void Effect::init()
{
    s_factories.reserve(16);
}

void Effect::shutdown()
{
    s_factories.clear();
    s_factories.compact();
}

template class List<Effect>;
template class Proxied<Effect>;
template class Array<Effect::FactoryInfo>;


}

