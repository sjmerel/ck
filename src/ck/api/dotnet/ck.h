#pragma once

#include "pathtype.h"


namespace CricketTechnology
{
namespace Audio
{

ref class Config;


/// <summary>
/// The Cricket Audio interface.
/// </summary>
public ref class Ck sealed
{
public:
    /// <summary>
    /// Initializes Cricket Audio. Returns nonzero if successful. 
    /// </summary>
    static int Init(Config^);

    /// <summary>
    /// Updates Cricket Audio.
    /// Should be called once per frame. 
    /// </summary>
    static void Update();

    /// <summary>
    /// Shuts down Cricket Audio. 
    /// </summary>
    static void Shutdown();

    /// <summary>
    /// Suspends Cricket Audio; typically used when an app is made inactive. 
    /// </summary>
    static void Suspend();

    /// <summary>
    /// Resumes Cricket Audio; typically used when an app is made active after having been inactive. 
    /// </summary>
    static void Resume();

    /// <summary>
    /// Returns a rough estimate of the load on the audio processing thread.
    /// This is the fraction of the interval between audio callbacks that was actually spent
    /// rendering audio; it will range between 0 and 1. 
    /// </summary>
    static property float RenderLoad
    {
        float get();
    }

    /// <summary>
    /// Returns the value of the clip flag.
    /// The clip flag is set to true whenever the final audio output "clips", i.e. 
    /// exceeds the maximum value. 
    /// </summary>
    static property bool ClipFlag
    {
        bool get();
    }

    /// <summary>
    /// Resets the value of the clip flag to false.  
    /// The clip flag is set to true whenever the final audio output "clips", i.e. 
    /// exceeds the maximum value. 
    /// </summary>
    static void ResetClipFlag();


    /// <summary>
    /// Gets/sets the maximum rate at which volumes change on playing sounds, in ms per full volume scale. 
    /// Default value is 40 ms. 
    /// </summary>
    static property float VolumeRampTime
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// Default value of volume ramp time. 
    /// </summary>
    static property float VolumeRampTimeMsDefault
    {
        float get();
    }


    /// <summary>
    /// Lock the audio processing thread to prevent it from processing any API calls.
    /// The lock should be held only for a short time.  This is useful to ensure that
    /// certain calls are processed together.  For example, if you want to play multiple
    /// sounds together exactly in sync, call LockAudio(), call Play() on each sound,
    /// then call UnlockAudio().
    /// </summary>
    static void LockAudio();

    /// <summary>
    /// Unlock the audio processing thread after a call to LockAudio(). 
    /// </summary>
    static void UnlockAudio();


    /// <summary>
    /// Start capturing the final audio output to a file.  The filename must end with ".wav" 
    /// (for a Microsoft WAVE file) or ".raw" (for a headerless file containing 16-bit interleaved
    /// stereo samples). 
    /// </summary>
    static void StartCapture(Platform::String^ path, PathType);

    /// <summary>
    /// Stop capturing audio output.
    /// </summary>
    static void StopCapture();

private:
    Ck();
};


}
}
