#include "ck/effectbus.h"
#include "ck/audio/effectbus.h"


CkEffectBus* CkEffectBus::newEffectBus()
{
    return (CkEffectBus*) Cki::EffectBus::newEffectBus();
}

CkEffectBus* CkEffectBus::getGlobalEffectBus()
{
    return (CkEffectBus*) Cki::EffectBus::getGlobalEffectBus();
}

