#pragma once

#include "ck/config.h"


namespace CricketTechnology
{
namespace Audio
{


/// <summary>
/// Log message types
/// </summary>
[Platform::Metadata::Flags]
public enum class LogType : uint32
{
    Info =    kCkLog_Info,
    Warning = kCkLog_Warning,
    Error =   kCkLog_Error,
    None =    kCkLog_None,
    All =     kCkLog_All
};


/// <summary>
/// Configuration options 
/// </summary>
public ref class Config sealed
{
public:

    /// <summary>
    /// Bitmask for which types of messages are logged. 
    ///  Default is LogType.All (all messages logged). 
    /// </summary>
    property uint32 LogMask
    {
        uint32 get();
        void set(uint32);
    }

    /// <summary>
    /// Audio processing frame duration, in ms.
    /// Shorter times result in lower latency, but require more CPU.
    /// Default is 5 ms. 
    /// </summary>
    property float AudioUpdateMs
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// Size of buffer for stream sounds, in ms.
    /// Larger buffers require more memory, but are less likely
    /// to skip due to slow file reads.
    /// Default is 500 ms. 
    /// </summary>
    property float StreamBufferMs
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// Interval between file reads for stream sounds, in ms.
    /// Shorter times will allow for smaller stream buffer sizes, but require more CPU.
    /// This value should be set to significantly less than StreamBufferMs, or your buffers will always underrun!
    /// Default is 100 ms. 
    /// </summary>
    property float StreamFileUpdateMs
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// Maximum number of audio tasks that can be queued at once.  Increase this
    /// from its default value of 500 if you see a warning stating "Couldn't produce
    /// task for audio graph thread". 
    /// </summary>
    property int MaxAudioTasks
    {
        int get();
        void set(int);
    }

    /// <summary>
    /// Maximum render load. When the value of Ck.RenderLoad exceeds this value,
    /// some sounds will not be rendered, to keep the CPU usage down.
    /// Default is 0.8.
    /// </summary>
    property float MaxRenderLoad
    {
        float get();
        void set(float);
    }

internal:
    property CkConfig* Impl
    {
        CkConfig* get() { return &m_cfg; }
    }

private:
    CkConfig m_cfg;

};


}
}

