#pragma once

#include "ck/core/platform.h"
#include "ck/core/string.h"

#include <vector>
#include <map>

namespace Cki
{


class ArgParser
{
public:
    ArgParser();
    ~ArgParser();

    // Add boolean flag option
    void addFlag(const char* name);

    // Add integer option
    void addIntOption(const char* name);

    // Add float option
    void addFloatOption(const char* name);

    // Add string option
    void addStringOption(const char* name);


    // Parse argv-style command line array
    bool parse(int argc, const char* const argv[]);


    // Returns true if flag was set 
    bool getFlag(const char* name) const;

    // Returns true if integer option was specified, and sets value
    bool getIntOption(const char* name, int&) const;

    // Returns true if float option was specified, and sets value
    bool getFloatOption(const char* name, float&) const;

    // Returns true if string option was specified, and sets value
    bool getStringOption(const char* name, const char*&) const;


    // Get the number of "extra" arguments (those that weren't part of the command line)
    int getNumExtraArgs() const;

    // Get an "extra" argument
    const char* getExtraArg(int i) const;


private:
    class Option
    {
    public:
        Option() : m_set(false) {}
        virtual ~Option() {}

        // Parse value; return number of args to skip, or -1 if error
        virtual int parse(const char*, int count) = 0;

        // If value is set, copy to pointer and return true; otherwise return false.
        virtual bool getValue(void*) = 0;

    protected:
        bool m_set;
    };

    class FlagOption : public Option
    {
    public:
        virtual int parse(const char*, int count);
        virtual bool getValue(void*);
    };

    class IntOption : public Option
    {
    public:
        virtual int parse(const char*, int count);
        virtual bool getValue(void*);
    private:
        int m_value;
    };

    class FloatOption : public Option
    {
    public:
        virtual int parse(const char*, int count);
        virtual bool getValue(void*);
    private:
        float m_value;
    };

    class StringOption : public Option
    {
    public:
        virtual int parse(const char*, int count);
        virtual bool getValue(void*);
    private:
        String m_value;
    };


    typedef std::map<String, Option*> OptionMap;
    OptionMap m_options;

    std::vector<String> m_extraArgs;


    ArgParser(const ArgParser&);
    ArgParser& operator=(const ArgParser&);

    void addOption(const char* name, Option* option);
    template <class T> bool getOption(const char* name, T& value) const;

};

}

