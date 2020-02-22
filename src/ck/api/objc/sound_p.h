#import "ck/objc/sound.h"

class CkSound;

@interface CkoSound (hidden)

- (void) destroy;
+ (CkoSound*) createWithImpl:(CkSound*)impl;
- (CkSound*) impl;

@end




