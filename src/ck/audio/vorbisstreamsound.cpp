#include "ck/audio/vorbisstreamsound.h"
#include "ck/core/path.h"
#include "ck/core/debug.h"

namespace Cki
{


VorbisStreamSound::VorbisStreamSound(const char* path, CkPathType pathType, int offset, int length) :
    m_stream(path, pathType, offset, length),
    m_source(&m_stream)
{
    subInit(&m_source);
}


}

