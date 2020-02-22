#import "ck/objc/mixer.h"
#import "ck/api/objc/nsstringref.h"
#import "ck/api/objc/proxy.h"
#import "ck/mixer.h"

@implementation CkoMixer
{
    CkMixer* m_impl;
}


- (void) setName:(NSString*) name
{
    NsStringRef str(name);
    m_impl->setName(str.getChars());
}

- (NSString*) name
{
    const char* name = m_impl->getName();
    return [NSString stringWithCString:name encoding:NSUTF8StringEncoding];
}

- (void) setVolume:(float) volume
{
    m_impl->setVolume(volume);
}

- (float) volume
{
    return m_impl->getVolume();
}

- (float) mixedVolume
{
    return m_impl->getMixedVolume();
}

- (void) setPaused:(BOOL) paused
{
    m_impl->setPaused(paused);
}

- (BOOL) paused
{
    return m_impl->isPaused();
}

- (BOOL) mixedPauseState
{
    return m_impl->getMixedPauseState();
}

- (void) setParent:(CkoMixer*)parent
{
    CkMixer* parentImpl = (parent ? parent->m_impl : NULL);
    m_impl->setParent(parentImpl);
}

- (CkoMixer*) parent
{
    return CkoMixerProxyGet(m_impl->getParent());
}

+ (CkoMixer*) master
{
    return CkoMixerProxyGet(CkMixer::getMaster());
}

+ (CkoMixer*) newMixer:(NSString*)name parent:(CkoMixer*)parent
{
    CkMixer* parentImpl = (parent ? parent->m_impl : NULL);
    NsStringRef str(name);
    CkMixer* impl = CkMixer::newMixer(str.getChars(), parentImpl);
    return CkoMixerProxyGet(impl);
}

+ (CkoMixer*) newMixer:(NSString*)name 
{
    return [CkoMixer newMixer:name parent:nil];
}

+ (CkoMixer*) find:(NSString*)name;
{
    NsStringRef str(name);
    CkMixer* impl = CkMixer::find(str.getChars());
    return CkoMixerProxyGet(impl);
}


////////////////////////////////////////
// overrides

- (void) dealloc
{
    if (m_impl)
    {
        CkoMixerProxyRemove(m_impl);
        m_impl->destroy();
        m_impl = NULL;
    }
    [super dealloc];
}

@end

////////////////////////////////////////
// private

@implementation CkoMixer (hidden)

- (void) destroy
{
    if (m_impl)
    {
        m_impl = NULL;
        [self release];
    }
}

+ (CkoMixer*) createWithImpl:(CkMixer*)impl
{
    CkoMixer* mixer = [[CkoMixer alloc] init];
    if (mixer)
    {
        mixer->m_impl = impl;
    }
    return mixer;
}

- (CkMixer*) impl
{
    return m_impl;
}

@end
