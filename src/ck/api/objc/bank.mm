#import "ck/objc/bank.h"
#import "ck/api/objc/proxy.h"
#import "ck/api/objc/nsstringref.h"
#import "ck/bank.h"

@implementation CkoBank
{
    CkBank* m_impl;
}


- (BOOL) loaded
{
    return m_impl->isLoaded();
}

- (BOOL) failed
{
    return m_impl->isFailed();
}

- (NSString*) name
{
    const char* name = m_impl->getName();
    return [NSString stringWithCString:name encoding:NSUTF8StringEncoding];
}

- (int) numSounds
{
    return m_impl->getNumSounds();
}

- (NSString*) soundName:(int) index
{
    const char* name = m_impl->getSoundName(index);
    return [NSString stringWithCString:name encoding:NSUTF8StringEncoding];
}


+ (CkoBank*) newBank:(NSString*)path 
{
    return [CkoBank newBank:path pathType:kCkPathType_Default];
}

+ (CkoBank*) newBank:(NSString*)path pathType:(CkPathType)pathType
{
    return [CkoBank newBank:path pathType:pathType offset:0 length:0];
}

+ (CkoBank*) newBank:(NSString*)path pathType:(CkPathType)pathType offset:(int)offset length:(int)length
{
    NsStringRef str(path);
    CkBank* impl = CkBank::newBank(str.getChars(), pathType, offset, length);
    return CkoBankProxyGet(impl);
}

+ (CkoBank*) newBankAsync:(NSString*)path 
{
    return [CkoBank newBankAsync:path pathType:kCkPathType_Default];
}

+ (CkoBank*) newBankAsync:(NSString*)path pathType:(CkPathType)pathType
{
    return [CkoBank newBankAsync:path pathType:pathType offset:0 length:0];
}

+ (CkoBank*) newBankAsync:(NSString*)path pathType:(CkPathType)pathType offset:(int)offset length:(int)length
{
    NsStringRef str(path);
    CkBank* impl = CkBank::newBankAsync(str.getChars(), pathType, offset, length);
    return CkoBankProxyGet(impl);
}

+ (CkoBank*) newBankFromMemory:(void*) buf bytes:(int)bytes
{
    CkBank* impl = CkBank::newBankFromMemory(buf, bytes);
    return CkoBankProxyGet(impl);
}

+ (CkoBank*) find:(NSString*)name
{
    NsStringRef str(name);
    CkBank* impl = CkBank::find(str.getChars());
    return CkoBankProxyGet(impl);
}


////////////////////////////////////////
// overrides

- (void) dealloc
{
    if (m_impl)
    {
        CkoBankProxyRemove(m_impl);
        m_impl->destroy();
        m_impl = NULL;
    }
    [super dealloc];
}

@end

////////////////////////////////////////
// private

@implementation CkoBank (hidden)

- (void) destroy
{
    if (m_impl)
    {
        m_impl = NULL;
        [self release];
    }
}

+ (CkoBank*) createWithImpl:(CkBank*)impl
{
    CkoBank* bank = [[CkoBank alloc] init];
    if (bank)
    {
        bank->m_impl = impl;
    }
    return bank;
}

- (CkBank*) impl
{
    return m_impl;
}

@end

