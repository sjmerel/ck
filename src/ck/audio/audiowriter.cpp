#include "ck/audio/audiowriter.h"
#include "ck/audio/audionode.h"
#include "ck/audio/audioutil.h"
#include "ck/core/mem.h"

namespace Cki
{


AudioWriter::AudioWriter(bool fixedPoint) :
    m_tmpBuf(NULL)
{
    if (fixedPoint)
    {
        m_tmpBuf = Mem::allocArray<float>(AudioNode::getBufferSamples());
    }
}

AudioWriter::~AudioWriter()
{
    Mem::free(m_tmpBuf);
}

int AudioWriter::write(const int32* buf, int samples)
{
    AudioUtil::convert(buf, m_tmpBuf, samples);
    return write(m_tmpBuf, samples);
}


}
