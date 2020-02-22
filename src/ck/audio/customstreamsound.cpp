#include "ck/audio/customstreamsound.h"
#include "ck/core/path.h"
#include "ck/core/debug.h"


namespace Cki
{


CustomStreamSound::CustomStreamSound(CkCustomStream* stream) :
    m_stream(stream),
    m_source(&m_stream)
{
    subInit(&m_source);
}


}

