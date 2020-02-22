#include "ck/audio/nativestreamsound_android_ics.h"
#include "ck/core/path.h"
#include "ck/core/debug.h"

namespace Cki
{


NativeStreamSoundIcs::NativeStreamSoundIcs(const char* path, bool isAsset) :
    m_stream(path, isAsset),
    m_source(&m_stream)
{
    subInit(&m_source);
}


}

