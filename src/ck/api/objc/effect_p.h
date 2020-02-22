#import "ck/objc/effect.h"
#import "ck/effect.h"

class CkBank;

@interface CkoEffect (hidden)

- (void) destroy;
+ (CkoEffect*) createWithImpl:(CkEffect*)impl;
- (CkEffect*) impl;

@end



