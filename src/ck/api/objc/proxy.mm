#include "ck/api/objc/proxy.h"
#include "ck/api/objc/mixer_p.h"
#include "ck/api/objc/sound_p.h"
#include "ck/api/objc/bank_p.h"
#include "ck/api/objc/effectbus_p.h"
#include "ck/api/objc/effect_p.h"
#include "ck/audio/mixer.h"
#include "ck/audio/sound.h"
#include "ck/audio/bank.h"
#include "ck/audio/effectbus.h"
#include "ck/audio/effect.h"
#include "ck/core/proxied.h"


namespace
{
    template <typename TPROXY, typename TIMPL>
    class ProxyInfo
    {
    public:
        void init()
        {
            m_proxies = CFDictionaryCreateMutable(NULL, 0, NULL, NULL);
            Cki::Proxied<TIMPL>::setProxyCallbacks(createCallback, destroyCallback, this);
        }

        void shutdown()
        {
            Cki::Proxied<TIMPL>::setProxyCallbacks(NULL, NULL, NULL);
            CFRelease(m_proxies);
        }

        TPROXY* getProxy(TIMPL* impl)
        {
            if (impl)
            {
                return (TPROXY*) CFDictionaryGetValue(m_proxies, impl);
            }
            else
            {
                return nil;
            }
        }

        void removeProxy(TIMPL* impl)
        {
            CFDictionaryRemoveValue(m_proxies, impl);
        }

    private:
        CFMutableDictionaryRef m_proxies;

        static void createCallback(TIMPL* impl, void* data)
        {
            ProxyInfo<TPROXY, TIMPL>* pi = (ProxyInfo<TPROXY, TIMPL>*) data;
            pi->create(impl);
        }

        void create(TIMPL* impl)
        {
            TPROXY* proxy = [TPROXY createWithImpl:impl];
            CFDictionaryAddValue(m_proxies, impl, proxy);
        }

        static void destroyCallback(TIMPL* impl, void* data)
        {
            ProxyInfo<TPROXY, TIMPL>* pi = (ProxyInfo<TPROXY, TIMPL>*) data;
            pi->destroy(impl);
        }

        void destroy(TIMPL* impl)
        {
            TPROXY* p = getProxy(impl);
            if (p)
            {
                [p destroy];
            }
            CFDictionaryRemoveValue(m_proxies, impl);
        }

    };

    ProxyInfo<CkoMixer, Cki::Mixer> g_mixerInfo;
    ProxyInfo<CkoSound, Cki::Sound> g_soundInfo;
    ProxyInfo<CkoBank, Cki::Bank> g_bankInfo;
    ProxyInfo<CkoEffectBus, Cki::EffectBus> g_effectBusInfo;
    ProxyInfo<CkoEffect, Cki::Effect> g_effectInfo;
}


////////////////////////////////////////

void CkoProxyInit()
{
    g_mixerInfo.init();
    g_soundInfo.init();
    g_bankInfo.init();
    g_effectBusInfo.init();
    g_effectInfo.init();
}

void CkoProxyShutdown()
{
    g_mixerInfo.shutdown();
    g_soundInfo.shutdown();
    g_bankInfo.shutdown();
    g_effectBusInfo.shutdown();
    g_effectInfo.shutdown();
}

////////////////////////////////////////

CkoMixer* CkoMixerProxyGet(CkMixer* impl)
{
    return g_mixerInfo.getProxy((Cki::Mixer*) impl);
}

void CkoMixerProxyRemove(CkMixer* impl)
{
    g_mixerInfo.removeProxy((Cki::Mixer*) impl);
}

CkoSound* CkoSoundProxyGet(CkSound* impl)
{
    return g_soundInfo.getProxy((Cki::Sound*) impl);
}

void CkoSoundProxyRemove(CkSound* impl)
{
    g_soundInfo.removeProxy((Cki::Sound*) impl);
}

CkoBank* CkoBankProxyGet(CkBank* impl)
{
    return g_bankInfo.getProxy((Cki::Bank*) impl);
}

void CkoBankProxyRemove(CkBank* impl)
{
    g_bankInfo.removeProxy((Cki::Bank*) impl);
}

CkoEffectBus* CkoEffectBusProxyGet(CkEffectBus* impl)
{
    return g_effectBusInfo.getProxy((Cki::EffectBus*) impl);
}

void CkoEffectBusProxyRemove(CkEffectBus* impl)
{
    g_effectBusInfo.removeProxy((Cki::EffectBus*) impl);
}

CkoEffect* CkoEffectProxyGet(CkEffect* impl)
{
    return g_effectInfo.getProxy((Cki::Effect*) impl);
}

void CkoEffectProxyRemove(CkEffect* impl)
{
    g_effectInfo.removeProxy((Cki::Effect*) impl);
}
