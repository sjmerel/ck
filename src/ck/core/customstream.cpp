#include "ck/core/customstream.h"

namespace Cki
{

CustomStream::CustomStream(CkCustomFile* file) :
    m_file(file)
{}

CustomStream::~CustomStream()
{
    delete m_file;
}

bool CustomStream::isValid() const
{
    return m_file->isValid();
}

int CustomStream::read(void* buf, int bytes)
{
    return m_file->read(buf, bytes);
}

int CustomStream::write(const void* buf, int bytes)
{
    return 0;
}

int CustomStream::getSize() const
{
    return m_file->getSize();
}

int CustomStream::getPos() const
{
    return m_file->getPos();
}

void CustomStream::setPos(int pos)
{
    m_file->setPos(pos);
}

void CustomStream::close()
{
    if (m_file)
    {
        delete m_file;
        m_file = NULL;
    }
}

}
