#include "argparser.h"
#include "ck/core/debug.h"
#include <stdio.h>

namespace Cki
{


int ArgParser::FlagOption::parse(const char* s, int count)
{
    m_set = true;
    return 0; // successful
}

bool ArgParser::FlagOption::getValue(void* value)
{
    if (m_set)
    {
        *((bool*)value) = true;
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////

int ArgParser::IntOption::parse(const char* s, int count)
{
    if (count >= 1 && sscanf(s, "%d", &m_value) == 1)
    {
        m_set = true;
        return 1; // successful
    }
    else
    {
        return -1;
    }
}

bool ArgParser::IntOption::getValue(void* value)
{
    if (m_set)
    {
        *((int*)value) = m_value;
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////

int ArgParser::FloatOption::parse(const char* s, int count)
{
    if (count >= 1 && sscanf(s, "%f", &m_value) == 1)
    {
        m_set = true;
        return 1; // successful
    }
    else
    {
        return -1;
    }
}

bool ArgParser::FloatOption::getValue(void* value)
{
    if (m_set)
    {
        *((float*)value) = m_value;
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////

int ArgParser::StringOption::parse(const char* s, int count)
{
    if (count >= 1)
    {
        m_value = s;
        m_set = true;
        return 1;
    }
    else
    {
        return -1;
    }

}

bool ArgParser::StringOption::getValue(void* value)
{
    if (m_set)
    {
        *((const char**)value) = m_value.getBuffer();
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////

ArgParser::ArgParser()
{
}

ArgParser::~ArgParser()
{
    for (OptionMap::iterator it = m_options.begin(); it != m_options.end(); ++it)
    {
        delete it->second;
    }
}

void ArgParser::addFlag(const char* name)
{
    addOption(name, new FlagOption());
}

void ArgParser::addIntOption(const char* name)
{
    addOption(name, new IntOption());
}

void ArgParser::addFloatOption(const char* name)
{
    addOption(name, new FloatOption());
}

void ArgParser::addStringOption(const char* name)
{
    addOption(name, new StringOption());
}

void ArgParser::addOption(const char* name, Option* option)
{
    OptionMap::iterator it = m_options.find(name);
    if (it != m_options.end())
    {
        delete it->second;
        it->second = option;
    }
    else
    {
        m_options.insert(std::make_pair(name, option));
    }
}

bool ArgParser::getFlag(const char* name) const
{
    bool value;
    return getOption(name, value);
}

bool ArgParser::getIntOption(const char* name, int& value) const
{
    return getOption(name, value);
}

bool ArgParser::getFloatOption(const char* name, float& value) const
{
    return getOption(name, value);
}

bool ArgParser::getStringOption(const char* name, const char*& value) const
{
    return getOption(name, value);
}

template <typename T>
bool ArgParser::getOption(const char* name, T& value) const
{
    // XXX this is sort of unsafe... if option is added as one type and queried as another,
    // will get garbage data.  should really do a type check here.
    OptionMap::const_iterator it = m_options.find(name);
    CK_ASSERT(it != m_options.end());
    return it->second->getValue(&value);
}

bool ArgParser::parse(int argc, const char* const argv[])
{
    m_extraArgs.clear();
    bool success = true;
    for (int i = 1; i < argc; ++i)
    {
        const char* arg = argv[i];
        if (arg[0] == '-')
        {
            const char* name = arg + 1;

            OptionMap::iterator it = m_options.find(name);
            if (it != m_options.end())
            {
                int skip = it->second->parse(argv[i+1], argc-i-1);
                if (skip < 0)
                {
                    CK_PRINT("Couldn't parse value for option %s\n", arg);
                    success = false;
                }
                else
                {
                    i += skip;
                }
            }
            else
            {
                CK_PRINT("Unknown option: %s\n", arg);
                success = false;
            }
        }
        else
        {
            m_extraArgs.push_back(arg);
        }
    }
    return success;
}

int ArgParser::getNumExtraArgs() const
{
    return (int) m_extraArgs.size();
}

const char* ArgParser::getExtraArg(int i) const
{
    return m_extraArgs[i].getBuffer();
}


}

