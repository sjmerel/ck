#include "ck/audio/fourcharcode.h"
#include "ck/core/binarystream.h"

namespace Cki
{

FourCharCode::FourCharCode()
{
    m_data[0] = 0;
    m_data[1] = 0;
    m_data[2] = 0;
    m_data[3] = 0;
}

FourCharCode::FourCharCode(char c0, char c1, char c2, char c3)
{
    m_data[0] = c0;
    m_data[1] = c1;
    m_data[2] = c2;
    m_data[3] = c3;
}

bool FourCharCode::operator==(const FourCharCode& other) const
{
    return m_data[0] == other.m_data[0] &&
           m_data[1] == other.m_data[1] &&
           m_data[2] == other.m_data[2] &&
           m_data[3] == other.m_data[3];
}

bool FourCharCode::operator!=(const FourCharCode& other) const
{
    return !(*this == other);
}

const char* FourCharCode::getData() const
{
    return m_data;
}

void FourCharCode::write(BinaryStream& stream) const
{
    stream.write(&m_data, sizeof(m_data));
}

void FourCharCode::read(BinaryStream& stream)
{
    stream.read(&m_data, sizeof(m_data));
}


}
