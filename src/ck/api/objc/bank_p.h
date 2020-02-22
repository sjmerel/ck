#import "ck/objc/bank.h"

class CkBank;

@interface CkoBank (hidden)

- (void) destroy;
+ (CkoBank*) createWithImpl:(CkBank*)impl;
- (CkBank*) impl;

@end


