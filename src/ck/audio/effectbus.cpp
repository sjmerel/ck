#include "ck/audio/effectbus.h"
#include "ck/audio/audiograph.h"
#include "ck/audio/graphsound.h"
#include "ck/core/logger.h"

// template definitions
#include "ck/core/proxied.cpp"
#include "ck/core/list.cpp"

namespace Cki
{


EffectBus::EffectBus(bool isOutput) :
    m_isOutput(isOutput),
    m_bypass(false),
    m_mixer(),
    m_fx(),
    m_outputBus(NULL)
{
    m_fx.setInput(&m_mixer);

    if (!isOutput)
    {
        MixNode* outMixer = AudioGraph::get()->getOutputMixer();
        AudioGraph::get()->execute(outMixer, MixNode::k_addInput, &m_fx);
    }
}

EffectBus::~EffectBus()
{
}

void EffectBus::addInput(AudioNode* node)
{
    AudioGraph::get()->execute(&m_mixer, MixNode::k_addInput, node);
}

void EffectBus::removeInput(AudioNode* node)
{
    AudioGraph::get()->execute(&m_mixer, MixNode::k_removeInput, node);
}

void EffectBus::addEffect(CkEffect* effect)
{
    AudioGraph::get()->execute(&m_fx, EffectBusNode::k_addEffect, effect);
}

void EffectBus::removeEffect(CkEffect* effect)
{
    AudioGraph::get()->execute(&m_fx, EffectBusNode::k_removeEffect, effect);
}

void EffectBus::removeAllEffects()
{
    AudioGraph::get()->execute(&m_fx, EffectBusNode::k_removeAllEffects);
}

void EffectBus::setOutputBus(CkEffectBus* outputBus)
{
    if (outputBus && (getGlobalEffectBus() == this))
    {
        CK_LOG_ERROR("Can't set the output on the global output effect bus");
        return;
    }

    CK_ASSERT(!outputBus || !m_isOutput);

    if (outputBus == this)
    {
        CK_LOG_ERROR("Can't set effect bus's output to itself");
        return;
    }
    // TODO: detect larger cycles

    if (outputBus != m_outputBus)
    {
        AudioGraph* graph = AudioGraph::get();
        MixNode* outMixer = graph->getOutputMixer();

        if (outputBus)
        {
            // detach from output mixer
            graph->execute(outMixer, MixNode::k_removeInput, &m_fx);

            // attach to output bus
            graph->execute(((EffectBus*) outputBus)->getInput(), MixNode::k_addInput, &m_fx);
        }
        else
        {
            // detach from output bus
            graph->execute(m_outputBus->getInput(), MixNode::k_removeInput, &m_fx);

            // attach to output mixer
            graph->execute(outMixer, MixNode::k_addInput, &m_fx);
        }

        if (outputBus)
        {
            ((EffectBus*) outputBus)->addInputBus(this);
        }

        if (m_outputBus)
        {
            m_outputBus->removeInputBus(this);
        }

        m_outputBus = (EffectBus*) outputBus;
    }
}

CkEffectBus* EffectBus::getOutputBus()
{
    return m_outputBus;
}

void EffectBus::reset()
{
    AudioGraph::get()->execute(&m_fx, EffectBusNode::k_reset);
}

void EffectBus::setBypassed(bool bypassed)
{
    AudioGraph::get()->execute(&m_fx, EffectBusNode::k_setBypassed, bypassed);
}

bool EffectBus::isBypassed() const
{
    return m_fx.isBypassed();
}

void EffectBus::setWetDryRatio(float wetDry)
{
    AudioGraph::get()->execute(&m_fx, EffectBusNode::k_setWetDryRatio, wetDry);
}

float EffectBus::getWetDryRatio() const
{
    return m_fx.getWetDryRatio();
}

void EffectBus::addInputSound(GraphSound* sound)
{
    m_inputSounds.addFirst(sound);
}

void EffectBus::removeInputSound(GraphSound* sound)
{
    m_inputSounds.remove(sound);
}

void EffectBus::addInputBus(EffectBus* bus)
{
    m_inputBuses.addFirst(bus);
}

void EffectBus::removeInputBus(EffectBus* bus)
{
    m_inputBuses.remove(bus);
}

MixNode* EffectBus::getInput()
{
    return &m_mixer;
}

AudioNode* EffectBus::getOutput()
{
    return &m_fx;
}

void EffectBus::destroy()
{
    if (getGlobalEffectBus() == this)
    {
        CK_LOG_ERROR("Can't destroy the global effect bus");
        return;
    }

    AudioGraph* graph = AudioGraph::get();

    graph->execute(&m_fx, EffectBusNode::k_removeAllEffects);
    graph->execute(&m_mixer, MixNode::k_detach);

    // unhook from output
    if (!m_isOutput)
    {
        MixNode* outMixer = graph->getOutputMixer();
        graph->execute(outMixer, MixNode::k_removeInput, &m_fx);
    }

    // remove this bus from all sounds
    GraphSound* inputSound = NULL;
    while ((inputSound = m_inputSounds.getFirst()))
    {
        inputSound->setEffectBus(NULL);
        m_inputSounds.remove(inputSound);
    }

    // detach from output bus
    if (m_outputBus)
    {
        graph->execute(m_outputBus->getInput(), MixNode::k_removeInput, &m_fx);
        m_outputBus->removeInputBus(this);
    }

    // remove this bus from all input buses
    EffectBus* inputBus = NULL;
    while ((inputBus = m_inputBuses.getFirst()))
    {
        inputBus->setOutputBus(NULL);
        m_inputBuses.remove(inputBus);
    }

    AudioGraph::get()->deleteObject(this);
}

EffectBus* EffectBus::newEffectBus()
{
    return new EffectBus();
}

EffectBus* EffectBus::getGlobalEffectBus()
{
    return AudioGraph::get()->getGlobalEffectBus();
}


template class List<GraphSound, 1>;
template class List<EffectBus>;
template class Proxied<EffectBus>;

}
