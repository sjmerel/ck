#include "ck/core/substream.h"
#include "ck/core/math.h"

namespace Cki
{


SubStream::SubStream(BinaryStream& stream, int offset, int length) :
    m_stream(stream),
    m_offset(offset),
    m_length(length),
    m_valid(true)
{
    if (offset + length > stream.getSize())
    {
        m_valid = false;
    }
    else
    {
        m_stream.setPos(m_offset);
    }
}

bool SubStream::isValid() const
{
    return m_valid && m_stream.isValid();
}

int SubStream::read(void* buf, int bytes)
{
    bytes = Math::min(bytes, getSize() - getPos());
    return m_stream.read(buf, bytes);
}

int SubStream::write(const void* buf, int bytes)
{
    bytes = Math::min(bytes, getSize() - getPos());
    return m_stream.write(buf, bytes);
}

int SubStream::getSize() const
{
    return m_length;
}

int SubStream::getPos() const
{
    return m_stream.getPos() - m_offset;
}

void SubStream::setPos(int pos)
{
    pos = Math::clamp(pos + m_offset, m_offset, m_offset + m_length);
    m_stream.setPos(pos);
}

void SubStream::close()
{
}



}
