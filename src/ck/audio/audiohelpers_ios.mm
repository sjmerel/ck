#include "ck/audio/audiohelpers_ios.h"
#include "ck/core/debug.h"

namespace
{
    NSAutoreleasePool* g_pool = nil;
}

namespace Cki
{

namespace AudioHelpers
{

    void createAutoreleasePool()
    {
        CK_ASSERT(!g_pool);
        g_pool = [[NSAutoreleasePool alloc] init];
    }

    void destroyAutoreleasePool()
    {
        CK_ASSERT(g_pool);
        [g_pool release];
        g_pool = nil;
    }

}

}
