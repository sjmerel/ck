#include "ck/core/binarywriter.h"
#include "ck/core/binarystream.h"
#include "ck/core/string.h"

namespace Cki
{


BinaryWriter::BinaryWriter(BinaryStream& stream) :
    m_stream(stream)
{
}

void BinaryWriter::write(const char* s)
{
    m_stream.write(s, String::getLength(s));
}


}
