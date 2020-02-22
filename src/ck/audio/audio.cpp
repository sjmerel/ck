#include "ck/audio/audio.h"
#include "ck/audio/graphsound.h"
#include "ck/audio/streamsource.h"
#include "ck/audio/mixer.h"
#include "ck/audio/effect.h"
#include "ck/audio/audionode.h"
#include "ck/audio/audiograph.h"
#include "ck/core/thread.h"
#include "ck/core/system.h"
#include <stdio.h>

#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
#  include "ck/audio/audiohelpers_ios.h"
#endif


namespace Cki
{

namespace Audio
{
    namespace
    {
        bool g_inited = false;
        bool g_stopFileThread = false;

        void* fileThreadFunc(void* arg)
        {
            int fileThreadMs = Math::round(System::get()->getConfig().streamFileUpdateMs);

            while (!g_stopFileThread)
            {
#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
                // need to create/destroy autorelease pools if using Cocoa objects
                // with autorelease!
                // TODO use @autoreleasepool {} instead
                AudioHelpers::createAutoreleasePool();
#endif
                // update file loading
                StreamSource::fileUpdateAll();

                // TODO use condition variable to wait if there are no stream sources

#if CK_PLATFORM_IOS || CK_PLATFORM_OSX || CK_PLATFORM_TVOS
                AudioHelpers::destroyAutoreleasePool();
#endif

                Thread::sleepMs(fileThreadMs);
            }
            return NULL;
        }

        Thread g_fileThread(fileThreadFunc);
    }

    // these are defined by the platform-specific code (e.g. audio_ios.cpp)
    bool platformInit();
    void platformShutdown();
    void platformUpdate();
    void platformSuspend();
    void platformResume();

    bool init()
    {
        if (!g_inited)
        {
            if (!platformInit())
			{
				return false;
			}

            g_stopFileThread = false;
#if CK_PLATFORM_ANDROID
            g_fileThread.setFlags(Thread::k_flagAttachToJvm);
#endif
            g_fileThread.setName("CK stream file");
            g_fileThread.start(NULL);

            Mixer::init();
            Effect::init();
            AudioNode::init();
            AudioGraph::init();

            g_inited = true;
        }

        return true;
    }

    void shutdown()
    {
        if (g_inited)
        {
            AudioGraph::get()->stop();
            AudioGraph::shutdown();
            AudioNode::shutdown();
            Effect::shutdown();
            Mixer::shutdown();

            platformShutdown();

            g_stopFileThread = true;
            g_fileThread.join();

            g_inited = false;
        }
    }

    void update()
    {
        // update playing state
        Sound::updateAll();
        platformUpdate();

        AudioGraph::get()->update();
    }

    void suspend()
    {
        platformSuspend();

        AudioGraph::get()->stop();
    }

    void resume()
    {
        platformResume();

        AudioGraph::get()->start();
    }

}


}
