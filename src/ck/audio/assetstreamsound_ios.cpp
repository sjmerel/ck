#include "ck/audio/assetstreamsound_ios.h"
#include "ck/core/debug.h"

namespace Cki
{


AssetStreamSound::AssetStreamSound(const char* url) :
    m_stream(url),
    m_source(&m_stream)
{
    subInit(&m_source);
}

}
