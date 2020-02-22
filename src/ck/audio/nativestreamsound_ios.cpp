#include "ck/audio/nativestreamsound_ios.h"
#include "ck/core/path.h"
#include "ck/core/debug.h"

namespace Cki
{


NativeStreamSound::NativeStreamSound(const char* path, int offset, int length) :
    m_stream(path, offset, length),
    m_source(&m_stream)
{
    subInit(&m_source);
}


}
