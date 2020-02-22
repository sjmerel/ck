#include "ck/audio/opensles_android.h"
#include "ck/core/debug.h"
#include <SLES/OpenSLES.h>
#include <dlfcn.h>

namespace Cki
{

namespace
{
    void* g_handle = NULL;

    typedef SLresult (*slCreateEngineFunc)(
            SLObjectItf             *pEngine,
            SLuint32                numOptions,
            const SLEngineOption    *pEngineOptions,
            SLuint32                numInterfaces,
            const SLInterfaceID     *pInterfaceIds,
            const SLboolean         *pInterfaceRequired
            );

    slCreateEngineFunc g_slCreateEngine = NULL;

}

namespace OpenSLES
{
    void init()
    {
        if (!g_handle)
        {
            g_handle = dlopen("libOpenSLES.so", RTLD_NOW | RTLD_GLOBAL);
            CK_ASSERT(g_handle);

            if (g_handle)
            {
                g_slCreateEngine = (slCreateEngineFunc) dlsym(g_handle, "slCreateEngine");
                CK_ASSERT(g_slCreateEngine);

#define GET_IID(iid) iid = *((SLInterfaceID*) dlsym(g_handle, #iid))

                GET_IID(SL_IID_NULL);
                GET_IID(SL_IID_OBJECT);
                GET_IID(SL_IID_AUDIOIODEVICECAPABILITIES);
                GET_IID(SL_IID_LED);
                GET_IID(SL_IID_VIBRA);
                GET_IID(SL_IID_METADATAEXTRACTION);
                GET_IID(SL_IID_METADATATRAVERSAL);
                GET_IID(SL_IID_DYNAMICSOURCE);
                GET_IID(SL_IID_OUTPUTMIX);
                GET_IID(SL_IID_PLAY);
                GET_IID(SL_IID_PREFETCHSTATUS);
                GET_IID(SL_IID_PLAYBACKRATE);
                GET_IID(SL_IID_SEEK);
                GET_IID(SL_IID_RECORD);
                GET_IID(SL_IID_EQUALIZER);
                GET_IID(SL_IID_VOLUME);
                GET_IID(SL_IID_DEVICEVOLUME);
                GET_IID(SL_IID_BUFFERQUEUE);
                GET_IID(SL_IID_PRESETREVERB);
                GET_IID(SL_IID_ENVIRONMENTALREVERB);
                GET_IID(SL_IID_EFFECTSEND);
                GET_IID(SL_IID_3DGROUPING);
                GET_IID(SL_IID_3DCOMMIT);
                GET_IID(SL_IID_3DLOCATION);
                GET_IID(SL_IID_3DDOPPLER);
                GET_IID(SL_IID_3DSOURCE);
                GET_IID(SL_IID_3DMACROSCOPIC);
                GET_IID(SL_IID_MUTESOLO);
                GET_IID(SL_IID_DYNAMICINTERFACEMANAGEMENT);
                GET_IID(SL_IID_MIDIMESSAGE);
                GET_IID(SL_IID_MIDIMUTESOLO);
                GET_IID(SL_IID_MIDITEMPO);
                GET_IID(SL_IID_MIDITIME);
                GET_IID(SL_IID_AUDIODECODERCAPABILITIES);
                GET_IID(SL_IID_AUDIOENCODERCAPABILITIES);
                GET_IID(SL_IID_AUDIOENCODER);
                GET_IID(SL_IID_BASSBOOST);
                GET_IID(SL_IID_PITCH);
                GET_IID(SL_IID_RATEPITCH);
                GET_IID(SL_IID_VIRTUALIZER);
                GET_IID(SL_IID_VISUALIZATION);
                GET_IID(SL_IID_ENGINE);
                GET_IID(SL_IID_ENGINECAPABILITIES);
                GET_IID(SL_IID_THREADSYNC);
                GET_IID(SL_IID_ANDROIDEFFECT);
                GET_IID(SL_IID_ANDROIDEFFECTSEND);
                GET_IID(SL_IID_ANDROIDEFFECTCAPABILITIES);
                GET_IID(SL_IID_ANDROIDCONFIGURATION);
                GET_IID(SL_IID_ANDROIDSIMPLEBUFFERQUEUE);
            }
        }
    }

    void shutdown()
    {
        if (g_handle)
        {
            CK_VERIFY( dlclose(g_handle) == 0 );
            g_handle = NULL;
        }
    }

    SLInterfaceID SL_IID_NULL = 0;
    SLInterfaceID SL_IID_OBJECT = 0;
    SLInterfaceID SL_IID_AUDIOIODEVICECAPABILITIES = 0;
    SLInterfaceID SL_IID_LED = 0;
    SLInterfaceID SL_IID_VIBRA = 0;
    SLInterfaceID SL_IID_METADATAEXTRACTION = 0;
    SLInterfaceID SL_IID_METADATATRAVERSAL = 0;
    SLInterfaceID SL_IID_DYNAMICSOURCE = 0;
    SLInterfaceID SL_IID_OUTPUTMIX = 0;
    SLInterfaceID SL_IID_PLAY = 0;
    SLInterfaceID SL_IID_PREFETCHSTATUS = 0;
    SLInterfaceID SL_IID_PLAYBACKRATE = 0;
    SLInterfaceID SL_IID_SEEK = 0;
    SLInterfaceID SL_IID_RECORD = 0;
    SLInterfaceID SL_IID_EQUALIZER = 0;
    SLInterfaceID SL_IID_VOLUME = 0;
    SLInterfaceID SL_IID_DEVICEVOLUME = 0;
    SLInterfaceID SL_IID_BUFFERQUEUE = 0;
    SLInterfaceID SL_IID_PRESETREVERB = 0;
    SLInterfaceID SL_IID_ENVIRONMENTALREVERB = 0;
    SLInterfaceID SL_IID_EFFECTSEND = 0;
    SLInterfaceID SL_IID_3DGROUPING = 0;
    SLInterfaceID SL_IID_3DCOMMIT = 0;
    SLInterfaceID SL_IID_3DLOCATION = 0;
    SLInterfaceID SL_IID_3DDOPPLER = 0;
    SLInterfaceID SL_IID_3DSOURCE = 0;
    SLInterfaceID SL_IID_3DMACROSCOPIC = 0;
    SLInterfaceID SL_IID_MUTESOLO = 0;
    SLInterfaceID SL_IID_DYNAMICINTERFACEMANAGEMENT = 0;
    SLInterfaceID SL_IID_MIDIMESSAGE = 0;
    SLInterfaceID SL_IID_MIDIMUTESOLO = 0;
    SLInterfaceID SL_IID_MIDITEMPO = 0;
    SLInterfaceID SL_IID_MIDITIME = 0;
    SLInterfaceID SL_IID_AUDIODECODERCAPABILITIES = 0;
    SLInterfaceID SL_IID_AUDIOENCODERCAPABILITIES = 0;
    SLInterfaceID SL_IID_AUDIOENCODER = 0;
    SLInterfaceID SL_IID_BASSBOOST = 0;
    SLInterfaceID SL_IID_PITCH = 0;
    SLInterfaceID SL_IID_RATEPITCH = 0;
    SLInterfaceID SL_IID_VIRTUALIZER = 0;
    SLInterfaceID SL_IID_VISUALIZATION = 0;
    SLInterfaceID SL_IID_ENGINE = 0;
    SLInterfaceID SL_IID_ENGINECAPABILITIES = 0;
    SLInterfaceID SL_IID_THREADSYNC = 0;
    SLInterfaceID SL_IID_ANDROIDEFFECT = 0;
    SLInterfaceID SL_IID_ANDROIDEFFECTSEND = 0;
    SLInterfaceID SL_IID_ANDROIDEFFECTCAPABILITIES = 0;
    SLInterfaceID SL_IID_ANDROIDCONFIGURATION = 0;
    SLInterfaceID SL_IID_ANDROIDSIMPLEBUFFERQUEUE = 0;

    SLresult slCreateEngine(
            SLObjectItf             *pEngine,
            SLuint32                numOptions,
            const SLEngineOption    *pEngineOptions,
            SLuint32                numInterfaces,
            const SLInterfaceID     *pInterfaceIds,
            const SLboolean         * pInterfaceRequired
            )
    {
        return g_slCreateEngine(pEngine, numOptions, pEngineOptions, numInterfaces, pInterfaceIds, pInterfaceRequired);
    }

}

}





