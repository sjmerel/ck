#include "ck/customfile.h"
#include "ck/core/readstream.h"

using namespace Cki;

void CkSetCustomFileHandler(CkCustomFileFunc handler, void* data)
{
    ReadStream::setFileHandler(handler, data);
}

