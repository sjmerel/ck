#include "ck/audio/wavwriter.h"
#include "ck/audio/fourcharcode.h"

namespace Cki
{


WavWriter::WavWriter(const char* path, int channels, int sampleRate, bool fixedPoint) :
    AudioWriter(fixedPoint),
    m_file(path, FileStream::k_writeTruncate)
{
    if (m_file.isValid())
    {
        // byte 0
        m_file << Cki::FourCharCode('R', 'I', 'F', 'F');
        m_file << (uint32) 0; // will be updated with size of WAVE chunk
        m_file << Cki::FourCharCode('W', 'A', 'V', 'E');

        // byte 12
        // format chunk
        m_file << Cki::FourCharCode('f', 'm', 't', ' ');
        m_file << (uint32) 16; // chunk size
        m_file << (uint16) 3; // format (float)
        m_file << (uint16) channels;
        m_file << (uint32) sampleRate;
        m_file << (uint32) (sampleRate * channels * sizeof(float)); // avgBytesPerSec
        m_file << (uint16) (channels * sizeof(float)); // block align
        m_file << (uint16) (8 * sizeof(float)); // bits per sample

        // byte 36
        // data chunk
        m_file << Cki::FourCharCode('d', 'a', 't', 'a');
        m_file << (uint32) (0); // will be updated with size of data chunk
    }
}

WavWriter::~WavWriter()
{
    close();
}

bool WavWriter::isValid() const
{
    return m_file.isValid();
}

int WavWriter::write(const float* buf, int samples)
{
    return m_file.write(buf, samples*sizeof(float)) / sizeof(float);
}

void WavWriter::close()
{
    if (m_file.isValid())
    {
        // update sizes in header
        int pos = m_file.getPos();
        int dataBytes = pos - 44; // 44 bytes for header
        m_file.setPos(4);
        m_file << (uint32) (dataBytes + 36);
        m_file.setPos(40);
        m_file << (uint32) dataBytes;
        m_file.close();
    }
}


}
