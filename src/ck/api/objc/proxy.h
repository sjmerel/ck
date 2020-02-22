#import "ck/mixer.h"
#import "ck/sound.h"
#import "ck/bank.h"
#import "ck/effectbus.h"
#import "ck/effect.h"
#import "ck/objc/mixer.h"
#import "ck/objc/sound.h"
#import "ck/objc/bank.h"
#import "ck/objc/effectbus.h"
#import "ck/objc/effect.h"

void CkoProxyInit();
void CkoProxyShutdown();

CkoMixer* CkoMixerProxyGet(CkMixer*);
void CkoMixerProxyRemove(CkMixer*);

CkoSound* CkoSoundProxyGet(CkSound*);
void CkoSoundProxyRemove(CkSound*);

CkoBank* CkoBankProxyGet(CkBank*);
void CkoBankProxyRemove(CkBank*);

CkoEffectBus* CkoEffectBusProxyGet(CkEffectBus*);
void CkoEffectBusProxyRemove(CkEffectBus*);

CkoEffect* CkoEffectProxyGet(CkEffect*);
void CkoEffectProxyRemove(CkEffect*);
