#pragma once

#include <stddef.h>
#include "effecttype.h"

class CkEffect;


namespace CricketTechnology
{
namespace Audio
{


/// <summary>
/// An audio effect. 
/// </summary>
public ref class Effect sealed
{
public:
    /// <summary>
    /// Set an effect parameter. 
    /// </summary>
    void SetParam(int paramId, float value);

    /// <summary>
    /// Reset the state of this effect. 
    /// </summary>
    void Reset();

    /// <summary>
    /// Get/set whether this effect is bypassed.
    /// A bypassed effect is not applied to the audio. 
    /// </summary>
    property bool Bypassed
    {
        bool get();
        void set(bool);
    }

    /// <summary>
    /// Gets/sets the wet/dry ratio for this effect
    /// The ratio can range from 0 to 1.
    /// A value of 0 means the audio is left completely "dry", that is, with no
    /// effects applied; it is equivalent to bypassing the ffect.
    /// A value of 1 means the audio is completely "wet", that is, the original
    /// "dry" signal is not mixed back into the processed audio. 
    /// </summary>
    property float WetDryRatio
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// Create a built-in effect. 
    /// </summary>
    static Effect^ NewEffect(EffectType);

    /// <summary>
    /// Create a custom effect (previously registered with the C++ API)
    /// </summary>
    static Effect^ NewCustomEffect(int id);

    /// <summary>
    /// Destroy the effect. 
    /// </summary>
    void Destroy();

    virtual ~Effect();

internal:
    Effect(CkEffect*);

    void Detach() { m_impl = NULL; }

    property CkEffect* Impl
    {
        CkEffect* get() { return m_impl; }
    }

private:
    CkEffect* m_impl;

};


}
}

