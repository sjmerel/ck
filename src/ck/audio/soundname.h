#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"

namespace Cki
{


class BinaryStream;
class MemoryFixup;

class SoundName
{
public:
    SoundName();

    SoundName(const char*);
    SoundName(const SoundName&);
    SoundName& operator=(const char*);
    SoundName& operator=(const SoundName&);

    enum { k_len = 256 };

    void set(const char*);

    int compare(const SoundName&) const;
    int compare(const char*) const;

    bool operator==(const SoundName&) const;
    bool operator!=(const SoundName&) const;
    bool operator<(const SoundName&) const;
    bool operator>(const SoundName&) const;
    bool operator<=(const SoundName&) const;
    bool operator>=(const SoundName&) const;

    bool operator==(const char*) const;
    bool operator!=(const char*) const;
    bool operator<(const char*) const;
    bool operator>(const char*) const;
    bool operator<=(const char*) const;
    bool operator>=(const char*) const;

    const char* getBuffer() const;

    // memory image
    void write(BinaryStream&) const;
    void fixup(MemoryFixup&) {}

private:
    char m_name[k_len];
};



}
