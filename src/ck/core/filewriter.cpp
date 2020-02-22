#include "ck/core/filewriter.h"
#include "ck/core/string.h"

namespace Cki
{


FileWriter::FileWriter(const char* path) :
    m_stream(path, FileStream::k_writeTruncate)
{}

void FileWriter::write(const char* s)
{
    m_stream.write(s, String::getLength(s));
}

bool FileWriter::isValid() const
{
    return m_stream.isValid();
}

void FileWriter::flush()
{
    m_stream.flush();
}

}
