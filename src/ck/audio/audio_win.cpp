#include "ck/audio/audio_win.h"
#include "ck/core/debug.h"
#include <stdio.h>

#if CK_PLATFORM_WIN
#  include "ck/audio/audiohelpers_win7.h"
#  include "ck/audio/audiohelpers_win8.h"
#else
#  include "ck/audio/audiohelpers_win.h"
#endif

#if CK_PLATFORM_WIN
  // GetVersion() is deprecated for Windows 8.1, but IsWindows8OrGreater() not available prior to 8.1
#  if _MSC_VER >= 1800
#    include <versionhelpers.h>
#  endif
#endif

#if CK_PLATFORM_WIN
#  define CALL_HELPER(x) if (g_win8) { return AudioHelpers8::x; } else { return AudioHelpers7::x; }
#else // WP8
#  define CALL_HELPER(x) return AudioHelpers::x;
#endif

namespace Cki
{

namespace
{
    bool g_win8 = false;
}

namespace Audio
{

    bool platformInit()
    {
#if CK_PLATFORM_WIN
#   if _MSC_VER >= 1800
        g_win8 = IsWindows8OrGreater();
#   else
        uint32 version = GetVersion();
        uint32 major = (version & 0x000000ff);
        uint32 minor = (version & 0x0000ff00) >> 8;
        g_win8 = (major >= 6 && minor >= 2);
#   endif
#endif

        CALL_HELPER(init());
    }

    void platformShutdown()
    {
        CALL_HELPER(shutdown());
    }

    void platformUpdate() {}

    void platformSuspend() {}

    void platformResume() {}


    void createVoice(BufferEndCallback cb, void* data)
    {
        CALL_HELPER(createVoice(cb, data));
    }

    void destroyVoice()
    {
        CALL_HELPER(destroyVoice());
    }

    void startVoice()
    {
        CALL_HELPER(startVoice());
    }

    void stopVoice()
    {
        CALL_HELPER(stopVoice());
    }

    void submitVoiceBuffer(float* buf, int bufSize)
    {
        CALL_HELPER(submitVoiceBuffer(buf, bufSize));
    }

}


}
