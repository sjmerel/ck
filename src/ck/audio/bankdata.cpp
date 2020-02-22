#include "ck/audio/bankdata.h"
#include "ck/core/binarystream.h"

// template definitions
#include "ck/core/array.cpp"

namespace Cki
{


BankData::BankData() :
    m_name("")
{
}

const SoundName& BankData::getName() const
{
    return m_name;
}

void BankData::setName(const char* name)
{
    m_name.set(name);
}

int BankData::getNumSamples() const
{
    return m_samples.getSize();
}

const Sample& BankData::getSample(int i) const
{
    return m_samples[i];
}

const Sample* BankData::findSample(const char* name) const
{
    // TODO use hashtable to speed up this search
    for (int i = 0; i < m_samples.getSize(); ++i)
    {
        if (m_samples[i].name == name)
        {
            return &m_samples[i];
        }
    }
    return NULL;
}

void BankData::addSample(const Sample& sample)
{
    m_samples.append(sample);
}

void BankData::write(BinaryStream& stream) const
{
    stream << m_name;
    stream << m_samples;
}

void BankData::fixup(MemoryFixup& fixup)
{
    m_name.fixup(fixup);
    m_samples.fixup(fixup);
}


template class Array<Sample>;


}
