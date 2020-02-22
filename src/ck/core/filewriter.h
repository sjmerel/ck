#pragma once

#include "ck/core/platform.h"
#include "ck/core/textwriter.h"
#include "ck/core/filestream.h"

namespace Cki
{


class FileWriter : public TextWriter
{
public:
    FileWriter(const char* path);

    virtual void write(const char*);

    bool isValid() const;
    void flush();

private:
    FileStream m_stream;

    FileWriter(const FileWriter&);
    FileWriter& operator=(const FileWriter&);
};


}

