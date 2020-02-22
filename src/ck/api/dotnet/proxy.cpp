#include "ck/core/proxied.h"
#include "ck/audio/bank.h"
#include "ck/audio/sound.h"
#include "ck/audio/mixer.h"
#include "ck/audio/effect.h"
#include "ck/audio/effectbus.h"
#include "bank.h"
#include "sound.h"
#include "mixer.h"
#include "effect.h"
#include "effectbus.h"
#include <collection.h>

using namespace CricketTechnology::Audio;
using namespace Platform::Collections;

namespace
{
    template <typename TPROXY, typename TIMPL>
    class ProxyInfo
    {
    public:
        void init()
        {
            m_proxies = ref new Map<uint64, TPROXY^>();
            Cki::Proxied<TIMPL>::setProxyCallbacks(createCallback, destroyCallback, this);
        }

        void shutdown()
        {
            Cki::Proxied<TIMPL>::setProxyCallbacks(NULL, NULL, NULL);
            m_proxies->Clear();
            m_proxies = nullptr;
        }

        TPROXY^ getProxy(TIMPL* impl)
        {
            if (impl)
            {
                return m_proxies->Lookup((uint64) impl);
            }
            else
            {
                return nullptr;
            }
        }

    private:
        Map<uint64, TPROXY^>^ m_proxies;

        static void createCallback(TIMPL* impl, void* data)
        {
            ProxyInfo<TPROXY, TIMPL>* pi = (ProxyInfo<TPROXY, TIMPL>*) data;
            pi->create(impl);
        }

        void create(TIMPL* impl)
        {
            TPROXY^ proxy = ref new TPROXY(impl);
            m_proxies->Insert((uint64) impl, proxy);
        }

        static void destroyCallback(TIMPL* impl, void* data)
        {
            ProxyInfo<TPROXY, TIMPL>* pi = (ProxyInfo<TPROXY, TIMPL>*) data;
            pi->destroy(impl);
        }

        void destroy(TIMPL* impl)
        {
            TPROXY^ p = getProxy(impl);
            if (p)
            {
                p->Detach();
                delete p;
            }
            m_proxies->Remove((uint64) impl);
        }

    };

    ProxyInfo<Mixer, Cki::Mixer> g_mixerInfo;
    ProxyInfo<Sound, Cki::Sound> g_soundInfo;
    ProxyInfo<Bank, Cki::Bank> g_bankInfo;
    ProxyInfo<EffectBus, Cki::EffectBus> g_effectBusInfo;
    ProxyInfo<Effect, Cki::Effect> g_effectInfo;
}


////////////////////////////////////////

namespace CricketTechnology 
{ 
namespace Audio 
{ 
namespace Proxy 
{

void Init()
{
    g_mixerInfo.init();
    g_soundInfo.init();
    g_bankInfo.init();
    g_effectBusInfo.init();
    g_effectInfo.init();
}

void Shutdown()
{
    g_mixerInfo.shutdown();
    g_soundInfo.shutdown();
    g_bankInfo.shutdown();
    g_effectBusInfo.shutdown();
    g_effectInfo.shutdown();
}

Mixer^ GetMixer(CkMixer* impl)
{
    return g_mixerInfo.getProxy((Cki::Mixer*) impl);
}

Bank^ GetBank(CkBank* impl)
{
    return g_bankInfo.getProxy((Cki::Bank*) impl);
}

Sound^ GetSound(CkSound* impl)
{
    return g_soundInfo.getProxy((Cki::Sound*) impl);
}

Effect^ GetEffect(CkEffect* impl)
{
    return g_effectInfo.getProxy((Cki::Effect*) impl);
}

EffectBus^ GetEffectBus(CkEffectBus* impl)
{
    return g_effectBusInfo.getProxy((Cki::EffectBus*) impl);
}

}
}
}
