#include "ck/effect.h"
#include "ck/audio/effect.h"


CkEffect* CkEffect::newEffect(CkEffectType type)
{
    return (CkEffect*) Cki::Effect::newEffect(type);
}

void CkEffect::registerCustomEffect(int id, CustomEffectFunc func)
{
    Cki::Effect::registerCustomEffect(id, func);
}

CkEffect* CkEffect::newCustomEffect(int id, void* arg)
{
    return (CkEffect*) Cki::Effect::newCustomEffect(id, arg);
}

