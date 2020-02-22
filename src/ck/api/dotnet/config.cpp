#include "config.h"

namespace CricketTechnology
{
namespace Audio
{


uint32 Config::LogMask::get()
{
    return m_cfg.logMask;
}

void Config::LogMask::set(uint32 mask)
{
    m_cfg.logMask = mask;
}

float Config::AudioUpdateMs::get()
{
    return m_cfg.audioUpdateMs;
}

void Config::AudioUpdateMs::set(float value)
{
    m_cfg.audioUpdateMs = value;
}

float Config::StreamBufferMs::get()
{
    return m_cfg.streamBufferMs;
}

void Config::StreamBufferMs::set(float value)
{
    m_cfg.streamBufferMs = value;
}

float Config::StreamFileUpdateMs::get()
{
    return m_cfg.streamFileUpdateMs;
}

void Config::StreamFileUpdateMs::set(float value)
{
    m_cfg.streamFileUpdateMs = value;
}

int Config::MaxAudioTasks::get()
{
    return m_cfg.maxAudioTasks;
}

void Config::MaxAudioTasks::set(int value)
{
    m_cfg.maxAudioTasks = value;
}

float Config::MaxRenderLoad::get()
{
    return m_cfg.maxRenderLoad;
}

void Config::MaxRenderLoad::set(float value)
{
    m_cfg.maxRenderLoad = value;
}


}
}
