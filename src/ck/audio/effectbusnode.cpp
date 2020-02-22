#include "ck/audio/effectbusnode.h"
#include "ck/audio/effect.h"
#include "ck/audio/audioutil.h"
#include "ck/effectprocessor.h"
#include "ck/core/debug.h"
#include "ck/core/mem.h"


namespace Cki
{


EffectBusNode::EffectBusNode() :
    m_input(NULL),
    m_bypass(false),
    m_wetDry(1.0f),
    m_prevActive(false)
{
}

BufferHandle EffectBusNode::processFloat(int frames, bool post, bool& needsPost)
{
    return process<float>(frames, post, needsPost);
}

BufferHandle EffectBusNode::processFixed(int frames, bool post, bool& needsPost)
{
    return process<int32>(frames, post, needsPost);
}

template <typename T>
BufferHandle EffectBusNode::process(int frames, bool post, bool& needsPost)
{
    BufferHandle inBufHandle = m_input ? m_input->process<T>(frames, post, needsPost) : BufferHandle();
    if (!inBufHandle.isValid())
    {
        if (m_prevActive)
        {
            reset();
        }
        m_prevActive = false;
        return inBufHandle;
    }
    else
    {
        m_prevActive = true;
    }

    float busWetDry = m_wetDry;
    if (m_bypass || m_effects.getSize() == 0 || busWetDry < 0.01f)
    {
        return inBufHandle;
    }

    BufferHandle busDryBuf;
    bool dryEnabled = (busWetDry < 0.99f);
    if (dryEnabled)
    {
        busDryBuf = acquireBuffer();
        Mem::copy(busDryBuf.get(), inBufHandle.get(), frames * 2 * sizeof(int32));
    }

    Effect* effect = m_effects.getFirst();
    while (effect)
    {
        if (!effect->isBypassed())
        {
            CkEffectProcessor* proc = effect->getProcessor();

            BufferHandle fxDryBuf;
            float fxWetDry = effect->getWetDryRatio();
            bool fxDryEnabled = (fxWetDry < 0.99f);
            if (fxDryEnabled)
            {
                fxDryBuf = acquireBuffer();
                Mem::copy(fxDryBuf.get(), inBufHandle.get(), frames * 2 * sizeof(int32));
            }

            BufferHandle outBufHandle;
            if (proc->isInPlace())
            {
                outBufHandle = inBufHandle;
            }
            else
            {
                outBufHandle = acquireBuffer();
            }

            proc->process((T*) inBufHandle.get(), (T*) outBufHandle.get(), frames);

            if (fxDryEnabled)
            {
                // equal-power crossfade
                float c = 0.5f * Math::cos(Math::k_pi * fxWetDry);
                float wetVol = Math::sqrt(0.5f - c);
                float dryVol = Math::sqrt(0.5f + c);
                AudioUtil::mixVol((const T*) fxDryBuf.get(), dryVol, (T*) outBufHandle.get(), wetVol, frames*2);
                fxDryBuf.release();
            }

            inBufHandle = outBufHandle;
        }

        effect = ((List<Effect>::Node*) effect)->getNext();
    }

    if (dryEnabled)
    {
        // equal-power crossfade
        float c = 0.5f * Math::cos(Math::k_pi * busWetDry);
        float wetVol = Math::sqrt(0.5f - c);
        float dryVol = Math::sqrt(0.5f + c);
        AudioUtil::mixVol((const T*) busDryBuf.get(), dryVol, (T*) inBufHandle.get(), wetVol, frames*2);
    }

    return inBufHandle;
}

void EffectBusNode::execute(int cmdId, CommandParam param0, CommandParam param1)
{
    switch (cmdId)
    {
        case k_addEffect:
            addEffect((Effect*) param0.addrValue);
            break;

        case k_removeEffect:
            removeEffect((Effect*) param0.addrValue);
            break;

        case k_removeAllEffects:
            removeAllEffects();
            break;

        case k_reset:
            reset();
            break;

        case k_setBypassed:
            setBypassed(param0.boolValue());
            break;

        case k_setWetDryRatio:
            setWetDryRatio(param0.floatValue);
            break;

        default:
            CK_FAIL("unknown command");
    };
}

#if CK_DEBUG
void EffectBusNode::print(int level)
{
    // TODO print effects
    printImpl(level, "Effect bus");
    if (m_input)
    {
        m_input->print(level+1);
    }
}
#endif

void EffectBusNode::addEffect(Effect* effect)
{
    m_effects.addLast(effect);
    effect->getProcessor()->reset();
    effect->setOwner(this);
}

void EffectBusNode::removeEffect(Effect* effect)
{
    m_effects.remove(effect);
    effect->setOwner(NULL);
}

void EffectBusNode::removeAllEffects()
{
    Effect* effect = m_effects.getFirst();
    while (effect)
    {
        effect->setOwner(NULL);
        effect = ((List<Effect>::Node*) effect)->getNext();
    }

    m_effects.clear();
}

void EffectBusNode::reset()
{
    Effect* effect = m_effects.getFirst();
    while (effect)
    {
        effect->getProcessor()->reset();
        effect = ((List<Effect>::Node*) effect)->getNext();
    }
}

void EffectBusNode::setBypassed(bool bypassed)
{
    if (bypassed && !m_bypass)
    {
        reset();
    }
    m_bypass = bypassed;
}

bool EffectBusNode::isBypassed() const
{
    return m_bypass;
}

void EffectBusNode::setWetDryRatio(float wetDry)
{
    m_wetDry = Math::clamp(wetDry, 0.0f, 1.0f);
}

float EffectBusNode::getWetDryRatio() const
{
    return m_wetDry;
}

void EffectBusNode::setInput(AudioNode* input)
{
    m_input = input;
}


}


