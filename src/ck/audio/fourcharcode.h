#pragma once

#include "ck/core/platform.h"

namespace Cki
{

class BinaryStream;

class FourCharCode
{
public:
    FourCharCode();
    FourCharCode(char c0, char c1, char c2, char c3);

    bool operator==(const FourCharCode& other) const;
    bool operator!=(const FourCharCode& other) const;

    const char* getData() const;

    void write(BinaryStream&) const;
    void read(BinaryStream&);
private:
    char m_data[4];
};



}
