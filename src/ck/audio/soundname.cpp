#include "ck/audio/soundname.h"
#include "ck/core/binarystream.h"
#include "ck/core/math.h"
#include "ck/core/logger.h"

namespace Cki
{


SoundName::SoundName()
{
    set("");
}

SoundName::SoundName(const char* name)
{
    set(name);
}

SoundName::SoundName(const SoundName& other) 
{
    set(other.m_name);
}

SoundName& SoundName::operator=(const char* str) 
{
    set(str);
    return *this;
}

SoundName& SoundName::operator=(const SoundName& other) 
{
    if (&other != this)
    {
        set(other.m_name);
    }
    return *this;
}

void SoundName::set(const char* str)
{
    // calculate length of str, up to k_len-1
    int len;
    for (len = 0; len < k_len-1; ++len)
    {
        if (!str[len])
        {
            break;
        }
    }

    if (len == k_len-1 && str[k_len-1])
    {
        CK_LOG_WARNING("Name \"%s\" is more than %d characters; truncating", str, k_len-1);
    }

    // copy
    for (int i = 0; i < len; ++i)
    {
        m_name[i] = str[i];
    }

    // zero out the rest
    for (int i = len; i < k_len; ++i)
    {
        m_name[i] = '\0';
    }
}

int SoundName::compare(const SoundName& other) const
{
    return compare(other.m_name);
}

int SoundName::compare(const char* str) const
{
    // faster than strncmp, for some reason

    for (int i = 0; i < k_len; ++i)
    {
        if (m_name[i] < str[i])
        {
            return -1;
        }
        else if (m_name[i] > str[i])
        {
            return 1;
        }
        else if (m_name[i] == 0 && str[i] == 0)
        {
            return 0;
        }
    }
    return 0;
}

bool SoundName::operator==(const SoundName& other) const
{
    return compare(other) == 0;
}

bool SoundName::operator!=(const SoundName& other) const
{
    return compare(other) != 0;
}

bool SoundName::operator<(const SoundName& other) const
{
    return compare(other) < 0;
}

bool SoundName::operator>(const SoundName& other) const
{
    return compare(other) > 0;
}

bool SoundName::operator<=(const SoundName& other) const
{
    return compare(other) <= 0;
}

bool SoundName::operator>=(const SoundName& other) const
{
    return compare(other) >= 0;
}

bool SoundName::operator==(const char* other) const
{
    return compare(other) == 0;
}

bool SoundName::operator!=(const char* other) const
{
    return compare(other) != 0;
}

bool SoundName::operator<(const char* other) const
{
    return compare(other) < 0;
}

bool SoundName::operator>(const char* other) const
{
    return compare(other) > 0;
}

bool SoundName::operator<=(const char* other) const
{
    return compare(other) <= 0;
}

bool SoundName::operator>=(const char* other) const
{
    return compare(other) >= 0;
}

const char* SoundName::getBuffer() const
{
    return m_name;
}

void SoundName::write(BinaryStream& stream) const
{
    stream.write(m_name, k_len);
}

}
