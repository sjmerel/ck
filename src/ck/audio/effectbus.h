#pragma once

#include "ck/effectbus.h"
#include "ck/core/platform.h"
#include "ck/core/allocatable.h"
#include "ck/core/deletable.h"
#include "ck/core/proxied.h"
#include "ck/core/list.h"
#include "ck/audio/mixnode.h"
#include "ck/audio/effectbusnode.h"

namespace Cki
{

class Effect;
class GraphSound;

class EffectBus : 
    public CkEffectBus,
    public Allocatable,
    public Deletable,
    public Proxied<EffectBus>,
    public List<EffectBus>::Node
{
public:
    EffectBus(bool isOutput = false);
    ~EffectBus();

    void destroy();

    // Ck::EffectBus:
    void addEffect(CkEffect*);
    void removeEffect(CkEffect*);
    void removeAllEffects();

    void setOutputBus(CkEffectBus*);
    CkEffectBus* getOutputBus();

    void reset();

    void setBypassed(bool);
    bool isBypassed() const;

    void setWetDryRatio(float);
    float getWetDryRatio() const;

    void addInput(AudioNode*);
    void removeInput(AudioNode*);

    void addInputSound(GraphSound*);
    void removeInputSound(GraphSound*);
    void addInputBus(EffectBus*);
    void removeInputBus(EffectBus*);

    MixNode* getInput();
    AudioNode* getOutput();

    static EffectBus* newEffectBus();
    static EffectBus* getGlobalEffectBus();

private:
    bool m_isOutput;
    bool m_bypass;
    MixNode m_mixer;
    EffectBusNode m_fx;
    EffectBus* m_outputBus;
    List<GraphSound, 1> m_inputSounds;
    List<EffectBus> m_inputBuses;

    EffectBus(const EffectBus&);
    EffectBus& operator=(const EffectBus&);
};


}



