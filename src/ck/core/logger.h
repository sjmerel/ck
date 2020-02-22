#pragma once

#include "ck/core/platform.h"
#include "ck/core/filewriter.h"
#include "ck/core/timer.h"
#include "ck/config.h"

namespace Cki
{


#define CK_LOG(type, format, ...)    do { Cki::g_logger.writef(type, format, ##__VA_ARGS__); } while (false)

#define CK_LOG_INFO(format, ...)     CK_LOG(kCkLog_Info,  format, ##__VA_ARGS__)
#define CK_LOG_WARNING(format, ...)  CK_LOG(kCkLog_Warning, format, ##__VA_ARGS__)
#define CK_LOG_ERROR(format, ...)    CK_LOG(kCkLog_Error, format, ##__VA_ARGS__)

#define CK_LOG_LIMITED(intervalMs, type, format, ...) \
    do \
    { \
        static Timer timer; \
        if (!timer.isRunning() || timer.getElapsedMs() > intervalMs) \
        { \
            timer.start(); \
            timer.reset(); \
            CK_LOG(type, format, ##__VA_ARGS__); \
        } \
    } while (false);


class Logger
{
public:
    Logger();

    void writef(CkLogType, const char* format, ...);
    void write(CkLogType, const char* msg);

    void setMask(uint32);
    void setFunc(CkLogFunc = NULL);
    void setFile(const char* path);

private:
    uint32 m_mask;
    CkLogFunc m_func;
    FileWriter* m_file;
    char m_fileMem[sizeof(FileWriter)];

    void closeFile();
    void write(TextWriter&, CkLogType, const char* msg);
};

extern Logger g_logger;


}
