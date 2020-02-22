#import "ck/objc/effectbus.h"
#import "ck/api/objc/effect_p.h"
#import "ck/api/objc/effectbus_p.h"
#import "ck/effectbus.h"
#import "ck/effect.h"
#import "ck/api/objc/proxy.h"

@implementation CkoEffectBus
{
    CkEffectBus* m_impl;
}


- (void) addEffect:(CkoEffect*)effect
{
    m_impl->addEffect([effect impl]);
}

- (void) removeEffect:(CkoEffect*)effect
{
    m_impl->removeEffect([effect impl]);
}

- (void) removeAllEffects
{
    m_impl->removeAllEffects();
}

- (void) setOutputBus:(CkoEffectBus*)bus
{
    m_impl->setOutputBus(bus ? [bus impl] : NULL);
}

- (CkoEffectBus*) outputBus
{
    return CkoEffectBusProxyGet(m_impl->getOutputBus());
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

+ (CkoEffectBus*) newEffectBus
{
    return CkoEffectBusProxyGet(CkEffectBus::newEffectBus());
}

+ (CkoEffectBus*) globalEffectBus
{
    return CkoEffectBusProxyGet(CkEffectBus::getGlobalEffectBus());
}

////////////////////////////////////////
// overrides

- (void) dealloc
{
    if (m_impl)
    {
        CkoEffectBusProxyRemove(m_impl);
        m_impl->destroy();
        m_impl = NULL;
    }
    [super dealloc];
}

@end

////////////////////////////////////////
// private

@implementation CkoEffectBus (hidden)

- (void) destroy
{
    if (m_impl)
    {
        m_impl = NULL;
        [self release];
    }
}

+ (CkoEffectBus*) createWithImpl:(CkEffectBus*)impl
{
    CkoEffectBus* bus = [[CkoEffectBus alloc] init];
    if (bus)
    {
        bus->m_impl = impl;
    }
    return bus;
}

- (CkEffectBus*) impl
{
    return m_impl;
}

@end

