#import "ck/objc/effectbus.h"

class CkEffectBus;

@interface CkoEffectBus (hidden)

- (void) destroy;
+ (CkoEffectBus*) createWithImpl:(CkEffectBus*)impl;
- (CkEffectBus*) impl;

@end


