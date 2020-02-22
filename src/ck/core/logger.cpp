#include "ck/core/logger.h"
#include "ck/core/debug.h"
#include <stdarg.h>
#include <stdio.h>
#include <new>

namespace Cki
{


Logger::Logger() :
    m_mask(kCkLog_All),
    m_func(NULL),
    m_file(NULL)
{
}

void Logger::writef(CkLogType type, const char* format, ...)
{
    if (type & m_mask)
    {
        const int k_bufSize = 256;
        char buf[k_bufSize];

        va_list args;
        va_start(args, format);
        CK_VERIFY( vsnprintf(buf, k_bufSize, format, args) >= 0 );
        write(type, buf);
        va_end(args);
    }
}

void Logger::write(CkLogType type, const char* msg)
{
    if (type & m_mask)
    {
        if (m_func)
        {
            m_func(type, msg);
        }
        else
        {
            write(g_debug, type, msg);
        }

        if (m_file)
        {
            write(*m_file, type, msg);
            m_file->flush();
        }
    }
}

void Logger::setMask(uint32 mask)
{
    m_mask = mask;
}

void Logger::setFunc(CkLogFunc func)
{
    m_func = func;
}

void Logger::setFile(const char* path)
{
    closeFile();

    if (path)
    {
        m_file = new (m_fileMem) FileWriter(path);
        if (!m_file->isValid())
        {
            CK_LOG_ERROR("log file \"%s\" could not be opened", path);
            closeFile();
        }
    }
}

////////////////////////////////////////

void Logger::closeFile()
{
    if (m_file)
    {
        m_file->~FileWriter();
        m_file = NULL;
    }

}

void Logger::write(TextWriter& writer, CkLogType type, const char* msg)
{
    const char* label = "";
    switch (type)
    {
        case kCkLog_Info:    label = "INFO";    break;
        case kCkLog_Warning: label = "WARNING"; break;
        case kCkLog_Error:   label = "ERROR";   break;
        default: break;
    }
    writer.writef("CK %s: %s\n", label, msg);
}

Logger g_logger;




}
