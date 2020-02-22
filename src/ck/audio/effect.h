#pragma once

#include "ck/core/platform.h"
#include "ck/core/list.h"
#include "ck/core/allocatable.h"
#include "ck/core/proxied.h"
#include "ck/core/deletable.h"
#include "ck/core/array.h"
#include "ck/audio/commandobject.h"
#include "ck/audio/bufferhandle.h"
#include "ck/audio/audiograph.h"
#include "ck/effect.h"

namespace Cki
{

class EffectBusNode;

class Effect : 
    public CkEffect,
    public CommandObject, 
    public Allocatable, 
    public Deletable, 
    public Proxied<Effect>, 
    public List<Effect>::Node
{
public:
    Effect(CkEffectProcessor*);
    virtual ~Effect();

    // CommandObject interface
    enum
    {
        k_setParam,
        k_reset
    };
    virtual void execute(int commandId, CommandParam param0, CommandParam param1);

    void setParam(int paramId, float value);

    void reset();

    void setBypassed(bool bypassed);
    bool isBypassed() const;

    void setWetDryRatio(float);
    float getWetDryRatio() const;

    CkEffectProcessor* getProcessor() { return m_proc; }
    void setOwner(EffectBusNode*);

    static Effect* newEffect(CkEffectType);

    static void registerCustomEffect(int id, CustomEffectFunc);
    static Effect* newCustomEffect(int id, void* arg);

    static void init();
    static void shutdown();

    void destroy();

private:
    CkEffectProcessor* m_proc;
    bool m_bypass;
    float m_wetDry;
    EffectBusNode* m_owner;

    struct FactoryInfo
    {
        int id;
        CustomEffectFunc func;

        void write(BinaryStream&) const {}
        void fixup(MemoryFixup&) {}
    };
    static Array<FactoryInfo> s_factories;
};


}

