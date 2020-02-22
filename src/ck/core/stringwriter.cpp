#include "ck/core/stringwriter.h"
#include "ck/core/string.h"

namespace Cki
{


StringWriter::StringWriter(String& s) 
    : m_string(s)
{
}

void StringWriter::write(const char* s)
{
    m_string.append(s);
}


}
