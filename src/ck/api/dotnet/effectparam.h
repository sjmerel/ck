#pragma once

#include "ck/effectparam.h"


namespace CricketTechnology
{
namespace Audio
{


/// <summary>
/// Parameters for the Biquad Filter effect 
/// </summary>
public enum class BiquadFilterParam
{
    /// <summary>
    /// Type of filter; should be one of the BiquadFilterParam_FilterType values. 
    /// </summary>
    FilterType = kCkBiquadFilterParam_FilterType,

    /// <summary>
    /// Center frequency of the filter. 
    /// </summary>
    Freq = kCkBiquadFilterParam_Freq,

    /// <summary>
    /// Q of the filter. 
    /// </summary>
    Q = kCkBiquadFilterParam_Q,

    /// <summary>
    /// Gain in dB (for Peak, LowShelf, and HighShelf filter types) 
    /// </summary>
    Gain = kCkBiquadFilterParam_Gain,

};


/// <summary>
/// Filter types, used for the BiquadFilterParam.FilterType param. 
/// </summary>
public enum class BiquadFilterParam_FilterType
{
    /// <summary>
    /// Low pass filter blocks high frequencies. 
    /// </summary>
    LowPass = kCkBiquadFilterParam_FilterType_LowPass,

    /// <summary>
    /// High pass filter blocks low frequencies. 
    /// </summary>
    HighPass = kCkBiquadFilterParam_FilterType_HighPass,

    /// <summary>
    /// Band pass filter blocks frequencies above and below the center frequency. 
    /// </summary>
    BandPass = kCkBiquadFilterParam_FilterType_BandPass,

    /// <summary>
    /// Notch filter blocks a narrow band of frequencies. 
    /// </summary>
    Notch = kCkBiquadFilterParam_FilterType_Notch,

    /// <summary>
    /// Peak filter boosts a narrow band of frequencies. 
    /// </summary>
    Peak = kCkBiquadFilterParam_FilterType_Peak,

    /// <summary>
    /// Low shelf filter boosts low frequencies. 
    /// </summary>
    LowShelf = kCkBiquadFilterParam_FilterType_LowShelf,

    /// <summary>
    /// High shelf filter boosts high frequencies. 
    /// </summary>
    HighShelf = kCkBiquadFilterParam_FilterType_HighShelf
};


////////////////////////////////////////


/// <summary>
/// Parameters for the Bit Crusher effect 
/// </summary>
public enum class BitCrusherParam
{
    /// <summary>
    /// Number of bits of resolution to keep in the input samples, in [1..24]. 
    /// </summary>
    BitResolution = kCkBitCrusherParam_BitResolution,

    /// <summary>
    /// Milliseconds to hold each sample value.  Higher values result in more
    /// reduction in the effective sample rate.  Values around 1 ms provide a reasonable effect. 
    /// </summary>
    HoldMs = kCkBitCrusherParam_HoldMs,
};


////////////////////////////////////////


/// <summary>
/// Parameters for the Ring Mod effect 
/// </summary>
public enum class RingModParam
{
    /// <summary>
    /// Frequency (Hz) of the modulating signal. Values around 500 Hz are typical. 
    /// </summary>
    Freq = kCkRingModParam_Freq,
};


////////////////////////////////////////


/// <summary>
/// Parameters for the Distortion effect
/// </summary>
public enum class DistortionParam
{
    /// <summary>
    /// Scale factor for input; larger values cause a more pronounced effect. 
    /// </summary>
    Drive = kCkDistortionParam_Drive,

    /// <summary>
    /// Offset for input.
    /// </summary>
    Offset = kCkDistortionParam_Offset,
};


}
}

