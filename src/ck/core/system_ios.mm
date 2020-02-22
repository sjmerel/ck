#include "ck/core/system_ios.h"
#include "ck/core/debug.h"
#include "ck/core/logger.h"
#include <new>
#if CK_PLATFORM_IOS
#  include <UIKit/UIKit.h>
#endif

////////////////////////////////////////
// dummy object so we can put Cocoa in multi-threaded mode

@interface DummyObject : NSObject
- (void) doNothing:(id)target;
@end

@implementation DummyObject
- (void) doNothing:(id)target 
{
    [self release];
}
@end

////////////////////////////////////////

namespace Cki
{



void SystemIos::init(const CkConfig& config)
{
    if (!s_instance)
    {
        s_instance = new (s_mem) SystemIos(config);
    }
}

void SystemIos::shutdown()
{
    if (s_instance)
    {
        s_instance->~SystemIos();
        s_instance = NULL;
    }
}

const char* SystemIos::getResourceDir() const
{
    return m_resourceDir;
}

const char* SystemIos::getTempDir() const
{
    return m_tempDir;
}

const char* SystemIos::getDocumentsDir() const
{
    return m_docDir;
}

#if CK_PLATFORM_IOS
bool SystemIos::isVersionGreaterThanOrEqualTo(const char* versionString) const
{
    NSString* v = [NSString stringWithUTF8String:versionString];
    return [[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending;
}
#endif

////////////////////////////////////////

SystemIos::SystemIos(const CkConfig& config) :
    System(config)
{
    NSString* resourceDir = [[NSBundle mainBundle] resourcePath];
    NSString* tempDir = NSTemporaryDirectory();
    NSString* docDir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];

    [resourceDir getCString:m_resourceDir maxLength:Path::k_maxLen encoding:NSUTF8StringEncoding];
    [tempDir getCString:m_tempDir maxLength:Path::k_maxLen encoding:NSUTF8StringEncoding];
    [docDir getCString:m_docDir maxLength:Path::k_maxLen encoding:NSUTF8StringEncoding];

    // Put Cocoa in multithreading mode by detaching a thread
    // (See "Advanced Memory Management Programming Guide:Using Autorelease Pools:Autorelease Pools and Threads")
    DummyObject* dummy = [[DummyObject alloc] init];
    [NSThread detachNewThreadSelector:@selector(doNothing:) toTarget:dummy withObject:nil];

//    CK_PRINT("resource dir: %s\n", m_resourceDir);
//    CK_PRINT("temp dir: %s\n", m_tempDir);
//    CK_PRINT("doc dir: %s\n", m_docDir);

#if CK_PLATFORM_IOS
    UIDevice* dev = [UIDevice currentDevice];
    CK_LOG_INFO("%s %s %s",
            [dev.model UTF8String],
            [dev.systemName UTF8String],
            [dev.systemVersion UTF8String]);

#if __ARM64_ARCH_8__
    CK_LOG_INFO("built for arm64");
#endif
#if __ARM_ARCH_7A__
    CK_LOG_INFO("built for armv7");
#endif
#if __ARM_ARCH_7S__
    CK_LOG_INFO("built for armv7s");
#endif
#if __i386__
    CK_LOG_INFO("built for x86");
#endif
#endif

    // all iOS devices so far support NEON; all 64-bit Intel macs support SSSE3.
    m_simd = true;
}

SystemIos::~SystemIos()
{
}

SystemIos* SystemIos::s_instance = NULL;
char SystemIos::s_mem[sizeof(SystemIos)];

}
