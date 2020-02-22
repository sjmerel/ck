#pragma once

#include <stddef.h>
#include "attenuationmode.h"
#include "pathtype.h"

class CkSound;


namespace CricketTechnology
{
namespace Audio
{

ref class Mixer;
ref class EffectBus;
ref class Bank;


/// <summary>
/// A sound (either memory-resident or streamed). 
/// </summary>
public ref class Sound sealed
{
public:
    /// <summary>
    /// The mixer to which this sound is assigned.
    /// Setting null will assign it to the master mixer. 
    /// </summary>
    property Mixer^ Mixer
    {
        CricketTechnology::Audio::Mixer^ get();
        void set(CricketTechnology::Audio::Mixer^);
    }

    /// <summary>
    /// Returns true if the sound is ready.
    /// Bank sounds are ready immediately after creation; stream sounds may not be. 
    /// </summary>
    property bool Ready
    {
        bool get();
    }

    /// <summary>
    /// Returns true if an error occurred while creating the sound. 
    /// </summary>
    property bool Failed
    {
        bool get();
    }

    /// <summary>
    /// Play the sound. 
    /// </summary>
    void Play();

    /// <summary>
    /// Stop the sound. 
    /// </summary>
    void Stop();

    /// <summary>
    /// Gets whether the sound is playing. 
    /// </summary>
    property bool Playing
    {
        bool get();
    }

    /// <summary>
    /// Gets/sets whether the sound is paused. 
    /// </summary>
    property bool Paused
    {
        bool get();
        void set(bool);
    }

    /// <summary>
    /// Gets/sets whether the sound is paused. 
    /// Returns true if the sound is pauses, its mixer is paused, or any of its
    /// mixer's ancestors are paused.
    /// </summary>
    property bool MixedPauseState
    {
        bool get();
    }

    /// <summary>
    /// Set the loop start and end.
    /// Default is to loop over all sample frames. 
    /// EndFrame is defined as one frame after the last frame of the loop.
    /// Setting endFrame = -1 is equivalent to setting it to this.Length.  
    /// </summary>
    void SetLoop(int startFrame, int endFrame);

    /// <summary>
    /// Get the loop start and end. 
    /// </summary>
    void GetLoop(int* startFrame, int* endFrame);

    /// <summary>
    /// Gets/sets the number of times the sound should loop.  
    /// 0 means no looping (play once and then stop).
    /// 1 means it will play twice; 2 means it will play 3 times; etc. 
    /// -1 means it will loop indefinitely until the sound is stopped or ReleaseLoop() is called. 
    /// </summary>
    property int LoopCount
    {
        int get();
        void set(int);
    }

    /// <summary>
    /// Gets the number of the current loop.
    /// For example, returns 0 if this is the first time playing through this sample;
    /// returns 1 if it is on its second loop through the sample; etc. 
    /// </summary>
    property int CurrentLoop
    {
        int get();
    }

    /// <summary>
    /// Makes the current loop the last.
    /// This is useful when you don't know in advance how many times you will
    /// want to loop; set the loop count to -1, and call ReleaseLoop() when
    /// you want it to stop looping and play to the end. 
    /// </summary>
    void ReleaseLoop();

    /// <summary>
    /// Returns true if ReleaseLoop() has been called after the last call to Play(). 
    /// </summary>
    property bool LoopReleased
    {
        bool get();
    }

    /// <summary>
    /// Get/set the play position in the sound, in sample frames.
    /// This can be called before playing, to start from an offset into
    /// the sound; or during playback, to jump to a new location. 
    /// </summary>
    property int PlayPosition
    {
        int get();
        void set(int);
    }

    /// <summary>
    /// Get/set the play position in the sound, in milliseconds.
    /// This can be called before playing, to start from an offset into
    /// the sound; or during playback, to jump to a new location. 
    /// </summary>
    property float PlayPositionMs
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// Gets/sets the volume.
    /// The volume can range from 0 to 1. The default value is 1. 
    /// </summary>
    property float Volume
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// Gets the volume value used for mixing. 
    /// This is the product of the volume set with this.Volume and the mixed
    /// volume value of the mixer to which the sound is assigned. 
    /// </summary>
    property float MixedVolume
    {
        float get();
    }

    /// <summary>
    /// Gets/sets the pan.
    /// The pan can range from -1 (left) to +1 (right). The default is 0 (center). 
    /// </summary>
    property float Pan
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// Sets the pan matrix explicitly.  
    /// The pan matrix determines how the left and right channels of an input are weighted during mixing.
    /// Usually it is set by this.Pan, but you can use SetPanMatrix() if you need more control.
    /// For a mono input, the left channel of the output is the input scaled by ll, and the right channel of the output is the input scaled by rr.  The off-diagonal lr and rl terms are ignored.
    /// For a stereo input, the left channel of the output is the left channel of the input scaled by ll plus the right channel of the input scaled by lr.  The right channel of the output is the left channel of the input scaled by rl plus the right channel of the input scaled by rr.
    /// </summary>
    void SetPanMatrix(float ll, float lr, float rl, float rr);

    /// <summary>
    /// Gets the pan matrix.
    /// The pan matrix determines how the left and right channels of an input are weighted during mixing.
    /// Usually it is set by this.Pan, but you can use SetPanMatrix() if you need more control.
    /// For a mono input, the left channel of the output is the input scaled by ll, and the right channel of the output is the input scaled by rr.  The off-diagonal lr and rl terms are ignored.
    /// For a stereo input, the left channel of the output is the left channel of the input scaled by ll plus the right channel of the input scaled by lr.  The right channel of the output is the left channel of the input scaled by rl plus the right channel of the input scaled by rr.
    /// </summary>
    void GetPanMatrix(float* ll, float* lr, float* rl, float* rr);

    /// <summary>
    /// Gets/sets the pitch shift value, in half-steps.
    /// This also changes the playback speed of the sound.  
    /// Positive values mean higher pitch (and faster playback); lower values mean lower pitch 
    /// (and slower playback).  The default value is 0. 
    /// </summary>
    property float PitchShift
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// Gets/sets the playback speed.
    /// Values greater than 1 mean faster playback; values less than 1 mean
    /// slower playback.  The default value is 1. 
    /// </summary>
    property float Speed
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// Gets/sets the sound to be played when this sound finishes playing.
    /// The next sound will play immediately, with no gaps. 
    /// </summary>
    property Sound^ NextSound
    {
        CricketTechnology::Audio::Sound^ get();
        void set(CricketTechnology::Audio::Sound^);
    }

    /// <summary>
    /// Gets the total duration of the sound, in sample frames.
    /// Stream sounds will return -1 if they are not ready or if the length is unknown. 
    /// </summary>
    property int Length
    {
        int get();
    }

    /// <summary>
    /// Gets the total duration of the sound, in milliseconds.
    /// Stream sounds will return -1 if they are not ready or if the length is unknown. 
    /// </summary>
    property float LengthMs
    {
        float get();
    }

    /// <summary>
    /// Gets the sample rate of the sound, in Hz.
    /// Stream sounds will return -1 if they are not ready. 
    /// </summary>
    property int SampleRate
    {
        int get();
    }

    /// <summary>
    /// Gets the number of channels in the sound (1 for mono, 2 for stereo).
    /// Stream sounds will return -1 if they are not ready. 
    /// </summary>
    property int Channels
    {
        int get();
    }

    /// <summary>
    /// Gets/sets the effect bus to which this sound's audio is sent to be processed, or
    /// null for dry output (no effects). 
    /// </summary>
    property EffectBus^ EffectBus
    {
        CricketTechnology::Audio::EffectBus^ get();
        void set(CricketTechnology::Audio::EffectBus^);
    }

    /// <summary>
    /// Sets whether 3D positioning is enabled.
    /// If enabled, pan will be determined by the sound and listener positions,
    /// ignoring the value set by this.Pan.  Volume will be attenuated based
    /// on the sound and listener positions and the attenuation settings. 
    /// </summary>
    property bool ThreeDEnabled
    {
        bool get();
        void set(bool);
    }

    /// <summary>
    /// Gets whether a 3D sound is virtual.
    /// If, due to distance attenuation, a 3D sound's volume becomes near enough to 0
    /// to be inaudible, it becomes "virtual".  A virtual sound is effectively paused
    /// and is not processed until it would become audible again; this reduces the 
    /// amount of audio processing required. 
    /// </summary>
    property bool Virtual
    {
        bool get();
    }

    /// <summary>
    /// Sets the position of the sound emitter in 3D space.
    /// This is used for pan and volume calculations when 3D positioning is enabled. 
    /// </summary>
    void Set3dPosition(float x, float y, float z);

    /// <summary>
    /// Gets the position of the sound emitter in 3D space.
    /// This is used for pan and volume calculations when 3D positioning is enabled. 
    /// </summary>
    void Get3dPosition(float* x, float* y, float* z);

    /// <summary>
    /// Sets the velocity of the sound emitter in 3D space.
    /// This is used for doppler shift calculations when 3D positioning is enabled.
    /// The units should be the same as those used for Sound.ThreeDSoundSpeed. 
    /// </summary>
    void Set3dVelocity(float vx, float vy, float vz);

    /// <summary>
    /// Gets the velocity of the sound emitter in 3D space.
    /// This is used for doppler shift calculations when 3D positioning is enabled.
    /// The units should be the same as those used for Sound.ThreeDSoundSpeed. 
    /// </summary>
    void Get3dVelocity(float* vx, float* vy, float* vz);

    /// <summary>
    /// Sets the listener position and rotation in 3D space. 
    /// This is used for pan and volume calculations when 3D positioning is enabled.
    /// </summary>
    /// <param name="eyeX">The listener's position (x)</param>
    /// <param name="eyeY">The listener's position (y)</param>
    /// <param name="eyeZ">The listener's position (z)</param>
    /// <param name="lookAtX">The listener's look-at point (x)</param>
    /// <param name="lookAtY">The listener's look-at point (y)</param>
    /// <param name="lookAtZ">The listener's look-at point (z)</param>
    /// <param name="upX">The listener's up vector (x)</param>
    /// <param name="upY">The listener's up vector (y)</param>
    /// <param name="upZ">The listener's up vector (z)</param>
    static void Set3dListenerPosition(float eyeX, float eyeY, float eyeZ,
                                      float lookAtX, float lookAtY, float lookAtZ,
                                      float upX, float upY, float upZ);

    /// <summary>
    /// Gets the listener position and rotation in 3D space. 
    /// This is used for pan and volume calculations when 3D positioning is enabled.
    /// </summary>
    /// <param name="eyeX">The listener's position (x)</param>
    /// <param name="eyeY">The listener's position (y)</param>
    /// <param name="eyeZ">The listener's position (z)</param>
    /// <param name="lookAtX">The listener's look-at point (x)</param>
    /// <param name="lookAtY">The listener's look-at point (y)</param>
    /// <param name="lookAtZ">The listener's look-at point (z)</param>
    /// <param name="upX">The listener's up vector (x)</param>
    /// <param name="upY">The listener's up vector (y)</param>
    /// <param name="upZ">The listener's up vector (z)</param>
    static void Get3dListenerPosition(float* eyeX, float* eyeY, float* eyeZ,
                                      float* lookAtX, float* lookAtY, float* lookAtZ,
                                      float* upX, float* upY, float* upZ);

    /// <summary>
    /// Sets the listener velocity in 3D space.
    /// This is used for doppler shift calculations when 3D positioning is enabled. 
    /// The units should be the same as those used for Sound.ThreeDSoundSpeed. 
    /// </summary>
    static void Set3dListenerVelocity(float vx, float vy, float vz);

    /// <summary>
    /// Gets the listener velocity in 3D space.
    /// This is used for doppler shift calculations when 3D positioning is enabled.
    /// The units should be the same as those used for Sound.ThreeDSoundSpeed. 
    /// </summary>
    static void Get3dListenerVelocity(float* vx, float* vy, float* vz);

    /// <summary>
    /// Sets parameters used for 3D volume attenuation. 
    /// </summary>
    /// <param name="mode">     The attenuation mode (default is AttenuationMode.InvDistanceSquared)</param>
    /// <param name="nearDist"> The near distance; sounds closer than this distance will not be attenuated (default is 1.0)</param>
    /// <param name="farDist">  The far distance; sounds further than this will be at the far volume (default is 100.0)</param>
    /// <param name="farVol">   The far volume; sounds further than farDist will be at this volume (default is 0.0)</param>
    static void Set3dAttenuation(AttenuationMode mode, float nearDist, float farDist, float farVol);

    /// <summary>
    /// Gets parameters used for 3D volume attenuation. 
    /// </summary>
    /// <param name="mode">     The attenuation mode</param>
    /// <param name="nearDist"> The near distance; sounds closer than this distance will not be attenuated.</param>
    /// <param name="farDist">  The far distance; sounds further than this will be at the far volume.</param>
    /// <param name="farVol">   The far volume; sounds further than farDist will be at this volume.</param>
    static void Get3dAttenuation(AttenuationMode* mode, float* nearDist, float* farDist, float* farVol);

    /// <summary>
    /// Gets/sets the speed of sound.
    /// This is used for doppler shift calculations when 3D positioning is enabled.  If no doppler
    /// shift is desired, this can be set to 0 (which is the default value).
    /// You may want to use the predefined constants for the speed of sound (such as 
    /// Sound.SoundSpeed_MetersPerSecond); use the constant that corresponds to
    /// the unit system you are using for your velocity and position values.
    /// You can also exaggerate the doppler effect by using a smaller value. 
    /// </summary>
    static property float ThreeDSoundSpeed
    {
        float get();
        void set(float);
    }

    /// <summary>
    /// The speed of sound in dry air at 20 degrees C, in centimeters per second.
    /// This value is provided as a convenience for Sound.ThreeDSoundSpeed. 
    /// </summary>
    static property float SoundSpeed_CentimetersPerSecond
    {
        float get();
    }

    /// <summary>
    /// The speed of sound in dry air at 20 degrees C, in meters per second.
    /// This value is provided as a convenience for Sound.ThreeDSoundSpeed. 
    /// </summary>
    static property float SoundSpeed_MetersPerSecond
    {
        float get();
    }

    /// <summary>
    /// The speed of sound in dry air at 20 degrees C, in inches per second.
    /// This value is provided as a convenience for Sound.ThreeDSoundSpeed. 
    /// </summary>
    static property float SoundSpeed_InchesPerSecond
    {
        float get();
    }

    /// <summary>
    /// The speed of sound in dry air at 20 degrees C, in feet per second.
    /// This value is provided as a convenience for Sound.ThreeDSoundSpeed. 
    /// </summary>
    static property float SoundSpeed_FeetPerSecond
    {
        float get();
    }

    /// <summary>
    /// Creates a sound from a bank by index. 
    /// </summary>
	[ Windows::Foundation::Metadata::DefaultOverload]
    static Sound^ NewBankSound(Bank^, int index);

    /// <summary>
    /// Creates a sound from a bank by name. 
    /// If bank is null, all loaded banks are searched for the first matching sound. 
    /// </summary>
    static Sound^ NewBankSound(Bank^, Platform::String^ name);

    /// <summary>
    /// Creates a stream from a file path. 
    /// Equivalent to NewStreamSound(path, PathType.Default). 
    /// </summary>
    static Sound^ NewStreamSound(Platform::String^ path);

    /// <summary>
    /// Creates a stream from a file path. 
    /// Equivalent to NewStreamSound(path, PathType.Default, 0, 0, null). 
    /// </summary>
    static Sound^ NewStreamSound(Platform::String^ path, PathType);

    /// <summary>
    /// Creates a stream from a file path. 
    /// If the stream file is embedded in a larger file, specify the offset and length of 
    /// the embedded data in bytes, and provide a string ending with a file extension that indicates
    /// the file format of the embedded file (for example, ".mp3" or "music.mp3").
    /// Otherwise, set the offset and length to 0 and the extension to null.
    /// </summary>
    static Sound^ NewStreamSound(Platform::String^ path, PathType, int offset, int length, Platform::String^ extension);

    /// <summary>
    /// Destroys the sound. 
    /// </summary>
    void Destroy();

    virtual ~Sound();

internal:
    Sound(CkSound*);

    void Detach() { m_impl = NULL; }

    property CkSound* Impl
    {
        CkSound* get() { return m_impl; }
    }

private:
    CkSound* m_impl;
};


}
}

