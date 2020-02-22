#include "ck/audio/cksstreamsound.h"
#include "ck/core/path.h"
#include "ck/core/debug.h"


namespace Cki
{


CksStreamSound::CksStreamSound(const char* path, CkPathType pathType, int offset, int length) :
    m_stream(path, pathType, offset, length),
    m_source(&m_stream)
{
    subInit(&m_source);
}

}
