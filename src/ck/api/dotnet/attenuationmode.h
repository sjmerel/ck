#pragma once

#include "ck/attenuationmode.h"


namespace CricketTechnology
{
namespace Audio
{


/// <summary>
/// Indicates how volume on 3D sounds is attenuated with distance. 
/// </summary>
public enum class AttenuationMode
{

    /// <summary>
    /// No attenuation; volume is 1.0 when closer than the far distance, and far volume outside of that. 
    /// </summary>
    None = kCkAttenuationMode_None,

    /// <summary>
    /// Linear attenuation of volume between near distance and far distance. 
    /// </summary>
    Linear = kCkAttenuationMode_Linear,

    /// <summary>
    /// Attenuation proportional to the reciprocal of the distance. 
    /// </summary>
    InvDistance = kCkAttenuationMode_InvDistance,

    /// <summary>
    /// Attenuation proportional to the square of the reciprocal of the distance. 
    /// </summary>
    ///
    /// <remarks>
    /// This is a good choice for realistically simulating point sound sources. 
    /// </remarks>
    InvDistanceSquared = kCkAttenuationMode_InvDistanceSquared

};


}
}

