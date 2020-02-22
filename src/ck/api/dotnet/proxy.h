#pragma once


class CkMixer; 
class CkSound; 
class CkBank; 
class CkEffect; 
class CkEffectBus;

namespace CricketTechnology 
{ 
namespace Audio 
{ 
    ref class Mixer; 
    ref class Sound; 
    ref class Bank; 
    ref class Effect; 
    ref class EffectBus; 

    namespace Proxy
    {
        void Init();
        void Shutdown();

        Mixer^     GetMixer(CkMixer*);
        Sound^     GetSound(CkSound*);
        Bank^      GetBank(CkBank*);
        Effect^    GetEffect(CkEffect*);
        EffectBus^ GetEffectBus(CkEffectBus*);
    }

} 
}
