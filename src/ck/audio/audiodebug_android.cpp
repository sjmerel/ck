#include "ck/audio/audiodebug_android.h"
#include "ck/audio/audio_android.h"
#include "ck/audio/opensles_android.h"
#include "ck/core/debug.h"
#include <SLES/OpenSLES_Android.h>

namespace Cki
{


#if CK_DEBUG
namespace Audio
{

#define CHECK_VALUE(value, nameValue) if (value == nameValue) { name = #nameValue; } else
    const char* getSlError(SLresult result)
    {
        const char* name = "(unknown)";
        CHECK_VALUE(result, SL_RESULT_PRECONDITIONS_VIOLATED);
        CHECK_VALUE(result, SL_RESULT_PARAMETER_INVALID);
        CHECK_VALUE(result, SL_RESULT_MEMORY_FAILURE);
        CHECK_VALUE(result, SL_RESULT_RESOURCE_ERROR);
        CHECK_VALUE(result, SL_RESULT_RESOURCE_LOST);
        CHECK_VALUE(result, SL_RESULT_IO_ERROR);
        CHECK_VALUE(result, SL_RESULT_BUFFER_INSUFFICIENT);
        CHECK_VALUE(result, SL_RESULT_CONTENT_CORRUPTED);
        CHECK_VALUE(result, SL_RESULT_CONTENT_UNSUPPORTED);
        CHECK_VALUE(result, SL_RESULT_CONTENT_NOT_FOUND);
        CHECK_VALUE(result, SL_RESULT_PERMISSION_DENIED);
        CHECK_VALUE(result, SL_RESULT_FEATURE_UNSUPPORTED);
        CHECK_VALUE(result, SL_RESULT_INTERNAL_ERROR);
        CHECK_VALUE(result, SL_RESULT_UNKNOWN_ERROR);
        CHECK_VALUE(result, SL_RESULT_OPERATION_ABORTED);
        CHECK_VALUE(result, SL_RESULT_CONTROL_LOST);
        ;
        return name;
    }

    const char* getInterfaceName(SLInterfaceID id)
    {
        const char* name = "(unknown)";
        CHECK_VALUE(id, OpenSLES::SL_IID_NULL);
        CHECK_VALUE(id, OpenSLES::SL_IID_OBJECT);
        CHECK_VALUE(id, OpenSLES::SL_IID_AUDIOIODEVICECAPABILITIES);
        CHECK_VALUE(id, OpenSLES::SL_IID_LED);
        CHECK_VALUE(id, OpenSLES::SL_IID_VIBRA);
        CHECK_VALUE(id, OpenSLES::SL_IID_METADATAEXTRACTION);
        CHECK_VALUE(id, OpenSLES::SL_IID_METADATATRAVERSAL);
        CHECK_VALUE(id, OpenSLES::SL_IID_DYNAMICSOURCE);
        CHECK_VALUE(id, OpenSLES::SL_IID_OUTPUTMIX);
        CHECK_VALUE(id, OpenSLES::SL_IID_PLAY);
        CHECK_VALUE(id, OpenSLES::SL_IID_PREFETCHSTATUS);
        CHECK_VALUE(id, OpenSLES::SL_IID_PLAYBACKRATE);
        CHECK_VALUE(id, OpenSLES::SL_IID_SEEK);
        CHECK_VALUE(id, OpenSLES::SL_IID_RECORD);
        CHECK_VALUE(id, OpenSLES::SL_IID_EQUALIZER);
        CHECK_VALUE(id, OpenSLES::SL_IID_VOLUME);
        CHECK_VALUE(id, OpenSLES::SL_IID_DEVICEVOLUME);
        CHECK_VALUE(id, OpenSLES::SL_IID_BUFFERQUEUE);
        CHECK_VALUE(id, OpenSLES::SL_IID_PRESETREVERB);
        CHECK_VALUE(id, OpenSLES::SL_IID_ENVIRONMENTALREVERB);
        CHECK_VALUE(id, OpenSLES::SL_IID_EFFECTSEND);
        CHECK_VALUE(id, OpenSLES::SL_IID_3DGROUPING);
        CHECK_VALUE(id, OpenSLES::SL_IID_3DCOMMIT);
        CHECK_VALUE(id, OpenSLES::SL_IID_3DLOCATION);
        CHECK_VALUE(id, OpenSLES::SL_IID_3DDOPPLER);
        CHECK_VALUE(id, OpenSLES::SL_IID_3DSOURCE);
        CHECK_VALUE(id, OpenSLES::SL_IID_3DMACROSCOPIC);
        CHECK_VALUE(id, OpenSLES::SL_IID_MUTESOLO);
        CHECK_VALUE(id, OpenSLES::SL_IID_DYNAMICINTERFACEMANAGEMENT);
        CHECK_VALUE(id, OpenSLES::SL_IID_MIDIMESSAGE);
        CHECK_VALUE(id, OpenSLES::SL_IID_MIDIMUTESOLO);
        CHECK_VALUE(id, OpenSLES::SL_IID_MIDITEMPO);
        CHECK_VALUE(id, OpenSLES::SL_IID_MIDITIME);
        CHECK_VALUE(id, OpenSLES::SL_IID_AUDIODECODERCAPABILITIES);
        CHECK_VALUE(id, OpenSLES::SL_IID_AUDIOENCODERCAPABILITIES);
        CHECK_VALUE(id, OpenSLES::SL_IID_AUDIOENCODER);
        CHECK_VALUE(id, OpenSLES::SL_IID_BASSBOOST);
        CHECK_VALUE(id, OpenSLES::SL_IID_PITCH);
        CHECK_VALUE(id, OpenSLES::SL_IID_RATEPITCH);
        CHECK_VALUE(id, OpenSLES::SL_IID_VIRTUALIZER);
        CHECK_VALUE(id, OpenSLES::SL_IID_VISUALIZATION);
        CHECK_VALUE(id, OpenSLES::SL_IID_ENGINE);
        CHECK_VALUE(id, OpenSLES::SL_IID_ENGINECAPABILITIES);
        CHECK_VALUE(id, OpenSLES::SL_IID_THREADSYNC);
        CHECK_VALUE(id, OpenSLES::SL_IID_ANDROIDEFFECT);
        CHECK_VALUE(id, OpenSLES::SL_IID_ANDROIDEFFECTSEND);
        CHECK_VALUE(id, OpenSLES::SL_IID_ANDROIDEFFECTCAPABILITIES);
        CHECK_VALUE(id, OpenSLES::SL_IID_ANDROIDCONFIGURATION);
        CHECK_VALUE(id, OpenSLES::SL_IID_ANDROIDSIMPLEBUFFERQUEUE);
        ;
        return name;
    }

    const char* getObjectName(SLuint32 id)
    {
        const char* name = "(unknown)";
        CHECK_VALUE(id, SL_OBJECTID_ENGINE);
        CHECK_VALUE(id, SL_OBJECTID_LEDDEVICE);
        CHECK_VALUE(id, SL_OBJECTID_VIBRADEVICE);
        CHECK_VALUE(id, SL_OBJECTID_AUDIOPLAYER);
        CHECK_VALUE(id, SL_OBJECTID_AUDIORECORDER);
        CHECK_VALUE(id, SL_OBJECTID_MIDIPLAYER);
        CHECK_VALUE(id, SL_OBJECTID_LISTENER);
        CHECK_VALUE(id, SL_OBJECTID_3DGROUP);
        CHECK_VALUE(id, SL_OBJECTID_OUTPUTMIX);
        CHECK_VALUE(id, SL_OBJECTID_METADATAEXTRACTOR);
        ;
        return name;
    }

    void printInterfaces(SLuint32 objId)
    {
        SLuint32 numInterfaces = 0;
        SLresult result = (*g_engine)->QueryNumSupportedInterfaces(g_engine, objId, &numInterfaces);
        if (result == SL_RESULT_FEATURE_UNSUPPORTED)
        {
            CK_PRINT("%s UNSUPPORTED\n", getObjectName(objId));
        }
        else
        {
            CK_PRINT("%s:\n", getObjectName(objId));
            for (int i = 0; i < numInterfaces; ++i)
            {
                SLInterfaceID intfId;
                CK_SL_VERIFY( (*g_engine)->QuerySupportedInterfaces(g_engine, objId, i, &intfId) );
                CK_PRINT("  %d: %s\n", i, getInterfaceName(intfId));
            }
        }
        CK_PRINT("\n");
    }

    void printInterfacesAndExtensions()
    {
        // print supported interfaces
        printInterfaces(SL_OBJECTID_ENGINE);
        printInterfaces(SL_OBJECTID_LEDDEVICE);
        printInterfaces(SL_OBJECTID_VIBRADEVICE);
        printInterfaces(SL_OBJECTID_AUDIOPLAYER);
        printInterfaces(SL_OBJECTID_AUDIORECORDER);
        printInterfaces(SL_OBJECTID_MIDIPLAYER);
        printInterfaces(SL_OBJECTID_LISTENER);
        printInterfaces(SL_OBJECTID_3DGROUP);
        printInterfaces(SL_OBJECTID_OUTPUTMIX);
        printInterfaces(SL_OBJECTID_METADATAEXTRACTOR);

        // print supported extensions
        SLuint32 numExtensions = 0;
        CK_SL_VERIFY( (*g_engine)->QueryNumSupportedExtensions(g_engine, &numExtensions) );
        CK_PRINT("extensions:\n");
        for (int i = 0; i < numExtensions; ++i)
        {
            SLint16 len;
            SLchar name[256];
            CK_SL_VERIFY( (*g_engine)->QuerySupportedExtension(g_engine, i, name, &len) );
            CK_PRINT("  %d: %s\n", i, name);
        }
    }

}
#endif 



}
