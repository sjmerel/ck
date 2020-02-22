#pragma once

#include "ck/core/platform.h"
#include "ck/core/textwriter.h"

namespace Cki
{

class String;

class StringWriter : public TextWriter
{
public:
    StringWriter(String&);

    virtual void write(const char*);

private:
    String& m_string;
};


}
