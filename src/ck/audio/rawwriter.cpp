#include "ck/audio/rawwriter.h"

namespace Cki
{


RawWriter::RawWriter(const char* path, bool fixedPoint) :
    AudioWriter(fixedPoint),
    m_file(path, FileStream::k_writeTruncate)
{
}

RawWriter::~RawWriter()
{
    close();
}

bool RawWriter::isValid() const
{
    return m_file.isValid();
}

int RawWriter::write(const float* buf, int samples)
{
    return m_file.write(buf, samples*sizeof(float)) / sizeof(float);
}

void RawWriter::close()
{
    m_file.close();
}


}
