#pragma once

#include "ck/core/platform.h"
#include "ck/core/textwriter.h"

namespace Cki
{


class BinaryStream;

class BinaryWriter : public TextWriter
{
public:
    BinaryWriter(BinaryStream&);

    virtual void write(const char*);

private:
    BinaryStream& m_stream;

    BinaryWriter(const BinaryWriter&);
    BinaryWriter& operator=(const BinaryWriter&);
};


}
