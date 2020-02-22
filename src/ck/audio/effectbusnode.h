#pragma once

#include "ck/core/platform.h"
#include "ck/audio/audionode.h"

namespace Cki
{

class Effect;

class EffectBusNode : public AudioNode
{
public:
    EffectBusNode();

    virtual BufferHandle processFloat(int frames, bool post, bool& needsPost);
    virtual BufferHandle processFixed(int frames, bool post, bool& needsPost);

    enum
    {
        k_addEffect,
        k_removeEffect,
        k_removeAllEffects,
        k_reset,
        k_setBypassed,
        k_setWetDryRatio
    };
    virtual void execute(int cmdId, CommandParam param0, CommandParam param1);

    void addEffect(Effect*);
    void removeEffect(Effect*);
    void removeAllEffects();

    void reset();

    void setBypassed(bool);
    bool isBypassed() const;

    void setWetDryRatio(float);
    float getWetDryRatio() const;

    void setInput(AudioNode*);

#if CK_DEBUG
    virtual void print(int level);
#endif

private:
    AudioNode* m_input;
    List<Effect> m_effects;
    bool m_bypass;
    float m_wetDry;
    bool m_prevActive;

    template <typename T> BufferHandle process(int frames, bool post, bool& needsPost);
};


}


