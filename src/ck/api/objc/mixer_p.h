#import "ck/objc/mixer.h"

class CkMixer;

@interface CkoMixer (hidden)

- (void) destroy;
+ (CkoMixer*) createWithImpl:(CkMixer*)impl;
- (CkMixer*) impl;

@end




