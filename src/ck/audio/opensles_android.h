#pragma once

#include <SLES/OpenSLES.h>

namespace Cki
{

// wrappers for OpenSLES functions and interface IDs; these are looked up with dlsym() 
// at runtime, so we don't have to link with libOpenSLES.so.

namespace OpenSLES
{
    void init();
    void shutdown();

    SLresult slCreateEngine(
            SLObjectItf             *pEngine,
            SLuint32                numOptions,
            const SLEngineOption    *pEngineOptions,
            SLuint32                numInterfaces,
            const SLInterfaceID     *pInterfaceIds,
            const SLboolean         *pInterfaceRequired
            );

    extern SLInterfaceID SL_IID_NULL;
    extern SLInterfaceID SL_IID_OBJECT;
    extern SLInterfaceID SL_IID_AUDIOIODEVICECAPABILITIES;
    extern SLInterfaceID SL_IID_LED;
    extern SLInterfaceID SL_IID_VIBRA;
    extern SLInterfaceID SL_IID_METADATAEXTRACTION;
    extern SLInterfaceID SL_IID_METADATATRAVERSAL;
    extern SLInterfaceID SL_IID_DYNAMICSOURCE;
    extern SLInterfaceID SL_IID_OUTPUTMIX;
    extern SLInterfaceID SL_IID_PLAY;
    extern SLInterfaceID SL_IID_PREFETCHSTATUS;
    extern SLInterfaceID SL_IID_PLAYBACKRATE;
    extern SLInterfaceID SL_IID_SEEK;
    extern SLInterfaceID SL_IID_RECORD;
    extern SLInterfaceID SL_IID_EQUALIZER;
    extern SLInterfaceID SL_IID_VOLUME;
    extern SLInterfaceID SL_IID_DEVICEVOLUME;
    extern SLInterfaceID SL_IID_BUFFERQUEUE;
    extern SLInterfaceID SL_IID_PRESETREVERB;
    extern SLInterfaceID SL_IID_ENVIRONMENTALREVERB;
    extern SLInterfaceID SL_IID_EFFECTSEND;
    extern SLInterfaceID SL_IID_3DGROUPING;
    extern SLInterfaceID SL_IID_3DCOMMIT;
    extern SLInterfaceID SL_IID_3DLOCATION;
    extern SLInterfaceID SL_IID_3DDOPPLER;
    extern SLInterfaceID SL_IID_3DSOURCE;
    extern SLInterfaceID SL_IID_3DMACROSCOPIC;
    extern SLInterfaceID SL_IID_MUTESOLO;
    extern SLInterfaceID SL_IID_DYNAMICINTERFACEMANAGEMENT;
    extern SLInterfaceID SL_IID_MIDIMESSAGE;
    extern SLInterfaceID SL_IID_MIDIMUTESOLO;
    extern SLInterfaceID SL_IID_MIDITEMPO;
    extern SLInterfaceID SL_IID_MIDITIME;
    extern SLInterfaceID SL_IID_AUDIODECODERCAPABILITIES;
    extern SLInterfaceID SL_IID_AUDIOENCODERCAPABILITIES;
    extern SLInterfaceID SL_IID_AUDIOENCODER;
    extern SLInterfaceID SL_IID_BASSBOOST;
    extern SLInterfaceID SL_IID_PITCH;
    extern SLInterfaceID SL_IID_RATEPITCH;
    extern SLInterfaceID SL_IID_VIRTUALIZER;
    extern SLInterfaceID SL_IID_VISUALIZATION;
    extern SLInterfaceID SL_IID_ENGINE;
    extern SLInterfaceID SL_IID_ENGINECAPABILITIES;
    extern SLInterfaceID SL_IID_THREADSYNC;
    extern SLInterfaceID SL_IID_ANDROIDEFFECT;
    extern SLInterfaceID SL_IID_ANDROIDEFFECTSEND;
    extern SLInterfaceID SL_IID_ANDROIDEFFECTCAPABILITIES;
    extern SLInterfaceID SL_IID_ANDROIDCONFIGURATION;
    extern SLInterfaceID SL_IID_ANDROIDSIMPLEBUFFERQUEUE;
}


}


