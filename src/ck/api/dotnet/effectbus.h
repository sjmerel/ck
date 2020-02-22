#pragma once

#include <stddef.h>

class CkEffectBus;


namespace CricketTechnology
{
namespace Audio
{

ref class Effect;


/// <summary>
/// Manages a set of audio effects applied to sounds. 
/// </summary>
public ref class EffectBus sealed
{
public:
    /// <summary>
    /// Add an effect to this bus.
    /// An effect can only be on one bus at a time. 
    /// </summary>
    void AddEffect(Effect^ effect);

    /// <summary>
    /// Remove an effect from this bus. 
    /// </summary>
    void RemoveEffect(Effect^ effect);

    /// <summary>
    /// Remove all effects from this bus. 
    /// </summary>
    void RemoveAllEffects();

    /// <summary>
    /// Get/set the bus to which this bus's output is sent.
    /// If null (the default), the audio from this bus is mixed into the final output. 
    /// </summary>
    property EffectBus^ OutputBus
    {
        EffectBus^ get();
        void set(EffectBus^);
    }

    /// <summary>
    /// Reset the state of all the effects on this bus. 
    /// </summary>
    void Reset();

    /// <summary>
    /// Get/set whether this bus is bypassed.
    /// A bypassed effect is not applied to the audio. 
    /// </summary>
    property bool Bypassed
    {
        bool get();
        void set(bool);
    }

    /// <summary>
    /// Gets/sets the wet/dry ratio for this bus.
    /// The ratio can range from 0 to 1.
    /// A value of 0 means the audio is left completely "dry", that is, with no
    /// effects applied; it is equivalent to bypassing the bus.
    /// A value of 1 means the audio is completely "wet", that is, the original
    /// "dry" signal is not mixed back into the processed audio. 
    /// </summary>
    property float WetDryRatio
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// Create a new effect bus. 
    /// </summary>
    static EffectBus^ NewEffectBus();

    /// <summary>
    /// Get the global effect bus.
    /// Effects on this bus are applied to the final audio before it is output. 
    /// </summary>
    static property EffectBus^ GlobalEffectBus
    {
        EffectBus^ get();
    }

    /// <summary>
    /// Destroy this effect bus. 
    /// </summary>
    void Destroy();

    virtual ~EffectBus();

internal:
    EffectBus(CkEffectBus*);

    void Detach() { m_impl = NULL; }

    property CkEffectBus* Impl
    {
        CkEffectBus* get() { return m_impl; }
    }

private:
    CkEffectBus* m_impl;
};


}
}

