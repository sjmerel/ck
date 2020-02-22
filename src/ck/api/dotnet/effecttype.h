#pragma once

#include "ck/effecttype.h"


namespace CricketTechnology
{
namespace Audio
{
    

/// <summary>
/// Types of built-in effects available 
/// </summary>
public enum class EffectType
{
    /// <summary>
    /// Biquadratic filter. 
    /// See BiquadFilterParam enum for parameter IDs. 
    /// </summary>
    BiquadFilter = kCkEffectType_BiquadFilter,

    /// <summary>
    /// Bit Crusher effect reduces bit resolution and/or bit rate, producing a retro
    /// low-fi 8-bit sound.
    /// See BitCrusherParam enum for parameter IDs. 
    /// </summary>
    BitCrusher = kCkEffectType_BitCrusher,

    /// <summary>
    /// Ring Mod effect modulates the audio by a sine wave. 
    /// See RingModParam enum for parameter IDs. 
    /// </summary>
    RingMod = kCkEffectType_RingMod,

    /// <summary>
    /// Distortion effect.
    /// See DistortionParam enum for parameter IDs. 
    /// </summary>
    Distortion = kCkEffectType_Distortion,
};


}
}
