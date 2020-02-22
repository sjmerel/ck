#pragma once

#include "ck/core/platform.h"
#include "ck/core/array.h"
#include "ck/audio/soundname.h"
#include "ck/audio/sample.h"

namespace Cki
{


class BinaryStream;
class MemoryFixup;


class BankData
{
public:
    BankData();

    const SoundName& getName() const;
    void setName(const char*);

    int getNumSamples() const;
    const Sample& getSample(int i) const;
    const Sample* findSample(const char* name) const;

    void addSample(const Sample&);

    // memory image
    void write(BinaryStream&) const;
    void fixup(MemoryFixup&);

//    static Bank* find(const char* name);
//    static const Sample* findSample(const char* name, Bank** bank);

private:
    SoundName m_name;
    Array<Sample> m_samples;

    BankData(const BankData&);
    BankData& operator=(const BankData&);
};


}
