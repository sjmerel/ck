#pragma once

#include <stddef.h>

class CkMixer;


namespace CricketTechnology
{
namespace Audio
{

ref class Mixer;


/// <summary>
/// A mixer represents a hierarchical grouping of sounds for controlling volumes. 
/// </summary>
public ref class Mixer sealed
{
public:
    /// <summary>
    /// Get/set the name of the mixer.
    /// The name should be 31 characters or less. 
    /// </summary>
    property Platform::String^ Name
    {
        Platform::String^ get();
        void set(Platform::String^);
    }

    /// <summary>
    /// Get/set the volume of this mixer.
    /// (The actual volume used during mixing will also depend on ancestors.) 
    /// </summary>
    property float Volume
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// Gets the volume used for mixing (affected by ancestors). 
    /// </summary>
    property float MixedVolume
    {
        float get();
    }

    /// <summary>
    /// Get/set whether this mixer is paused.
    /// A sound will be paused if the sound, its mixer, or any of its mixer's ancestors
    /// are paused. 
    /// </summary>
    property bool Paused
    {
        bool get();
        void set(bool);
    }

    /// <summary>
    /// Returns true if this mixer, or any of its ancestors, is paused.
    /// </summary>
    property bool MixedPauseState
    {
        bool get();
    }

    /// <summary>
    /// Set the parent of the mixer.
    /// Setting to null sets the master mixer as the parent. 
    /// </summary>
    property Mixer^ Parent
    {
        Mixer^ get();
        void set(Mixer^);
    }

    /// <summary>
    /// Returns the master mixer, which is the root of the mixer hierarchy. 
    /// </summary>
    static property Mixer^ Master
    {
        Mixer^ get();
    }

    /// <summary>
    /// Creates a new mixer. 
    /// </summary>
    static Mixer^ NewMixer(Platform::String^ name, Mixer^ parent);

    /// <summary>
    /// Creates a new mixer. 
    /// Equivalent to NewMixer(name, null). 
    /// </summary>
    static Mixer^ NewMixer(Platform::String^ name);

    /// <summary>
    /// Finds a mixer by name; returns null if not found. 
    /// </summary>
    static Mixer^ Find(Platform::String^ name);

    /// <summary>
    /// Destroys the mixer. 
    /// </summary>
    void Destroy();

    virtual ~Mixer();

internal:
    Mixer(CkMixer*);

    void Detach() { m_impl = NULL; }

    property CkMixer* Impl
    {
        CkMixer* get() { return m_impl; }
    }

private:
    CkMixer* m_impl;
};


}
}


