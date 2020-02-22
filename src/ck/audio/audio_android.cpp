#include "ck/audio/audio_android.h"
#include "ck/audio/audiodebug_android.h"
#include "ck/audio/audiotrackproxy_android.h"
#include "ck/audio/opensles_android.h"
#include "ck/audio/nativestreamsound_android.h"
#include "ck/core/system_android.h"
#include "ck/core/logger.h"
#include "ck/core/debug.h"

namespace Cki
{

namespace
{
    bool g_useOpenSL = false;
    bool g_useAudioTrack = false;
    int g_nativeSampleRate = 0; //48000;
}

namespace Audio
{

    SLObjectItf g_engineObj = NULL;
    SLEngineItf g_engine = NULL;
    SLObjectItf g_outputMixObj = NULL;

    int getNativeSampleRate() 
    { 
        CK_ASSERT(g_nativeSampleRate > 0);
        return g_nativeSampleRate; 
    }

    bool platformInit()
    {
        // if SDK < 9, can't use OpenSL.
        // if SDK >= 9, will use OpenSL for compressed streams;
        // unless config.useJavaAudio is also set, it will use OpenSL for mixed audio output.
        SystemAndroid* system = SystemAndroid::get();
        g_useOpenSL = system->getSdkVersion() >= 9;
        g_useAudioTrack = system->getSdkVersion() < 9 || system->getConfig().useJavaAudio;

        if (g_useOpenSL)
        {

            // init OpenSLES stubs
            OpenSLES::init();

            // create engine
            CK_SL_VERIFY( OpenSLES::slCreateEngine(&g_engineObj, 0, NULL, 0, NULL, NULL) );
            CK_SL_VERIFY( (*g_engineObj)->Realize(g_engineObj, SL_BOOLEAN_FALSE) );

            // get the engine interface, which is needed in order to create other objects
            CK_SL_VERIFY( (*g_engineObj)->GetInterface(g_engineObj, OpenSLES::SL_IID_ENGINE, &g_engine) );

            // create output mix
            CK_SL_VERIFY( (*g_engine)->CreateOutputMix(g_engine, &g_outputMixObj, 0, NULL, NULL) );
            CK_SL_VERIFY( (*g_outputMixObj)->Realize(g_outputMixObj, SL_BOOLEAN_FALSE) );

#if CK_DEBUG
            //        printInterfacesAndExtensions();
#endif
        }

        AudioTrackProxy::init();
        g_nativeSampleRate = AudioTrackProxy::getNativeOutputSampleRate();
        CK_LOG_INFO("Native output sample rate is %d", g_nativeSampleRate);
        if (!g_useAudioTrack)
        {
            AudioTrackProxy::shutdown();
        }

        if (g_useAudioTrack)
        {
            CK_LOG_INFO("Using Java audio for output");
        }
        else
        {
            CK_LOG_INFO("Using OpenSL ES for output");
        }

        return true;
    }

    void platformShutdown()
    {
        if (g_useOpenSL)
        {
            (*g_outputMixObj)->Destroy(g_outputMixObj);
            (*g_engineObj)->Destroy(g_engineObj);
            g_engineObj = NULL;
            g_engine = NULL;

            OpenSLES::shutdown();
        }
        
        if (g_useAudioTrack)
        {
            AudioTrackProxy::shutdown();
        }
    }

    void platformUpdate()
    {
    }

    void platformSuspend()
    {
        NativeStreamSound::suspendAll();
    }

    void platformResume() 
    {
        NativeStreamSound::resumeAll();
    }

}


}

