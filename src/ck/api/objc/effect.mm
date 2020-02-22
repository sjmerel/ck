#import "ck/objc/effect.h"
#import "ck/effect.h"
#import "ck/api/objc/proxy.h"

@implementation CkoEffect
{
    CkEffect* m_impl;
}


- (void) setParam:(int)paramId value:(float)value
{
    m_impl->setParam(paramId, value);
}

- (void) reset
{
    m_impl->reset();
}

- (void) setBypassed:(BOOL)bypass
{
    m_impl->setBypassed(bypass);
}

- (BOOL) bypassed
{
    return m_impl->isBypassed();
}

- (void) setWetDryRatio:(float)wetDry
{
    m_impl->setWetDryRatio(wetDry);
}

- (float) wetDryRatio
{
    return m_impl->getWetDryRatio();
}

+ (CkoEffect*) newEffect:(CkEffectType)type;
{
    CkEffect* effectImpl = CkEffect::newEffect(type);
    return CkoEffectProxyGet(effectImpl);
}

+ (CkoEffect*) newCustomEffect:(int)id;
{
    CkEffect* effectImpl = CkEffect::newCustomEffect(id);
    return CkoEffectProxyGet(effectImpl);
}

////////////////////////////////////////
// overrides

- (void) dealloc
{
    if (m_impl)
    {
        CkoEffectProxyRemove(m_impl);
        m_impl->destroy();
        m_impl = NULL;
    }
    [super dealloc];
}

@end

////////////////////////////////////////
// private

@implementation CkoEffect (hidden)

- (void) destroy
{
    if (m_impl)
    {
        m_impl = NULL;
        [self release];
    }
}

+ (CkoEffect*) createWithImpl:(CkEffect*)impl
{
    CkoEffect* effect = [[CkoEffect alloc] init];
    if (effect)
    {
        effect->m_impl = impl;
    }
    return effect;
}

- (CkEffect*) impl
{
    return m_impl;
}

@end


