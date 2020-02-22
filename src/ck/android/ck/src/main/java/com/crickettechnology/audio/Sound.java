package com.crickettechnology.audio;

import java.util.HashMap;

/** A sound (either memory-resident or streamed). */

public final class Sound extends Proxy
{
    /** Sets the mixer to which this sound is assigned.
      Setting null will assign it to the master mixer. */
    public void setMixer(Mixer mixer)
    {
        nativeSetMixer(m_inst, mixer == null ? 0 : mixer.m_inst);
    }

    /** Gets the mixer to which this sound is assigned. */
    public Mixer getMixer()
    {
        return Mixer.getMixer(nativeGetMixer(m_inst));
    }

    /** Returns true if the sound is ready.
      Bank sounds are ready immediately after creation; stream sounds may not be. */
    public boolean isReady()
    {
        return nativeIsReady(m_inst);
    }

    /** Returns true if an error occurred while creating the sound. */
    public boolean isFailed()
    {
        return nativeIsFailed(m_inst);
    }

    /** Play the sound. */
    public void play()
    {
        nativePlay(m_inst);
    }

    /** Stop the sound. */
    public void stop()
    {
        nativeStop(m_inst);
    }

    /** Gets whether the sound is playing. */
    public boolean isPlaying()
    {
        return nativeIsPlaying(m_inst);
    }

    /** Sets whether the sound is paused.
     Note that even if false, the sound will be paused if its mixer or any of its 
     mixer's ancestors are paused.*/
    public void setPaused(boolean paused)
    {
        nativeSetPaused(m_inst, paused);
    }

    /** Gets whether the sound is paused.
     Note that even if false, the sound will be paused if its mixer or any of its 
     mixer's ancestors are paused.*/
    public boolean isPaused()
    {
        return nativeIsPaused(m_inst);
    }

    /** Returns true if the sound is pauses, its mixer is paused, or any of its
      mixer's ancestors are paused. */
    public boolean getMixedPauseState()
    {
        return nativeGetMixedPauseState(m_inst);
    }

    /** Set the loop start and end.
      Default is to loop over all sample frames. 
      EndFrame is defined as one frame after the last frame of the loop.
      Setting endFrame = -1 is equivalent to setting it to getLength().
      <p>
      This has no effect for streams in formats other than .cks or Ogg Vorbis. */
    public void setLoop(int startFrame, int endFrame)
    {
        nativeSetLoop(m_inst, startFrame, endFrame);
    }

    /** Get the loop start and end.
      Default is to loop over all sample frames. 
      EndFrame is defined as one frame after the last frame of the loop.
      Setting endFrame = -1 is equivalent to setting it to getLength().
      <p>
      This has no effect for streams in formats other than .cks or Ogg Vorbis. */
    public void getLoop(IntRef startFrame, IntRef endFrame)
    {
        nativeGetLoop(m_inst, startFrame, endFrame);
    }

    /** Sets the number of times the sound should loop.  
      <ul>
      <li>0 means no looping (play once and then stop).</li>
      <li>1 means it will play twice; 2 means it will play 3 times; etc. </li>
      <li>-1 means it will loop indefinitely until the sound is stopped or releaseLoop() is called.</li>
      </ul>
      */
    public void setLoopCount(int loopCount)
    {
        nativeSetLoopCount(m_inst, loopCount);
    }

    /** Gets the number of times the sound should loop.  
      <ul>
      <li>0 means no looping (play once and then stop).</li>
      <li>1 means it will play twice; 2 means it will play 3 times; etc. </li>
      <li>-1 means it will loop indefinitely until the sound is stopped or releaseLoop() is called.</li>
      </ul>
      */
    public int getLoopCount()
    {
        return nativeGetLoopCount(m_inst);
    }

    /** Gets the number of the current loop.
      For example, returns 0 if this is the first time playing through this sample;
      returns 1 if it is on its second loop through the sample; etc. */
    public int getCurrentLoop()
    {
        return nativeGetCurrentLoop(m_inst);
    }

    /** Makes the current loop the last.
      This is useful when you don't know in advance how many times you will
      want to loop; set the loop count to -1, and call releaseLoop() when
      you want it to stop looping and play to the end. */
    public void releaseLoop()
    {
        nativeReleaseLoop(m_inst);
    }

    /** Returns true if releaseLoop() has been called after the last call to play(). */
    public boolean isLoopReleased()
    {
        return nativeIsLoopReleased(m_inst);
    }

    /** Set the play position in the sound, in sample frames.
      This can be called before playing, to start from an offset into
      the sound; or during playback, to jump to a new location. 
      <p>
      This is not supported for streams in formats other than .cks or Ogg Vorbis; use setPlayPositionMs() instead when possible. */
    public void setPlayPosition(int frame)
    {
        nativeSetPlayPosition(m_inst, frame);
    }

    /** Set the play position in the sound, in milliseconds.
      This can be called before playing, to start from an offset into
      the sound; or during playback, to jump to a new location. */
    public void setPlayPositionMs(float ms)
    {
        nativeSetPlayPositionMs(m_inst, ms);
    }

    /** Get the current play position in the sound, in sample frames. 
      <p>
      This is not supported for streams in formats other than .cks or Ogg Vorbis; use getPlayPositionMs() instead when possible. */
    public int getPlayPosition()
    {
        return nativeGetPlayPosition(m_inst);
    }

    /** Get the current play position in the sound, in milliseconds. */
    public float getPlayPositionMs()
    {
        return nativeGetPlayPositionMs(m_inst);
    }

    /** Sets the volume.
      The volume can range from 0 to 1. The default value is 1. */
    public void setVolume(float volume)
    {
        nativeSetVolume(m_inst, volume);
    }

    /** Gets the volume.
      The volume can range from 0 to 1. The default value is 1. */
    public float getVolume()
    {
        return nativeGetVolume(m_inst);
    }

    /** Gets the volume value used for mixing. 
      This is the product of the volume set with setVolume() and the mixed
      volume value of the mixer to which the sound is assigned. */
    public float getMixedVolume()
    {
        return nativeGetMixedVolume(m_inst);
    }

    /** Sets the pan.
      The pan can range from -1 (left) to +1 (right). The default is 0 (center). */
    public void setPan(float pan)
    {
        nativeSetPan(m_inst, pan);
    }

    /** Gets the pan.
      The pan can range from -1 (left) to +1 (right). The default is 0 (center). */
    public float getPan()
    {
        return nativeGetPan(m_inst);
    }

    /** Sets the pan matrix explicitly.  
      The pan matrix determines how the left and right channels of an input are weighted during mixing.
      Usually it is set by setPan(), but you can use setPanMatrix() if you need more control.
      For a mono input, the left channel of the output is the input scaled by ll, and the right channel of the output is the input scaled by rr.  The off-diagonal lr and rl terms are ignored.
      For a stereo input, the left channel of the output is the left channel of the input scaled by ll plus the right channel of the input scaled by lr.  The right channel of the output is the left channel of the input scaled by rl plus the right channel of the input scaled by rr.
      */
    public void setPanMatrix(float ll, float lr, float rl, float rr)
    {
        nativeSetPanMatrix(m_inst, ll, lr, rl, rr);
    }

    /** Gets the pan matrix.
      The pan matrix determines how the left and right channels of an input are weighted during mixing.
      Usually it is set by setPan(), but you can use setPanMatrix() if you need more control.
      For a mono input, the left channel of the output is the input scaled by ll, and the right channel of the output is the input scaled by rr.  The off-diagonal lr and rl terms are ignored.
      For a stereo input, the left channel of the output is the left channel of the input scaled by ll plus the right channel of the input scaled by lr.  The right channel of the output is the left channel of the input scaled by rl plus the right channel of the input scaled by rr.
      */
    public void getPanMatrix(FloatRef ll, FloatRef lr, FloatRef rl, FloatRef rr) 
    {
        nativeGetPanMatrix(m_inst, ll, lr, rl, rr);
    }

    /** Sets the pitch shift value, in half-steps.
      This also changes the playback speed of the sound.  
      Positive values mean higher pitch (and faster playback); lower values mean lower pitch 
      (and slower playback).  The default value is 0. 
      <p>
      This does not work for streams in formats other than .cks or Ogg Vorbis. */
    public void setPitchShift(float halfSteps)
    {
        nativeSetPitchShift(m_inst, halfSteps);
    }

    /** Gets the pitch shift value, in half-steps.
      Positive values mean higher pitch (and faster playback); lower values mean lower pitch 
      (and slower playback).  The default value is 0. 
      <p>
      This does not work for streams in formats other than .cks or Ogg Vorbis. */
    public float getPitchShift()
    {
        return nativeGetPitchShift(m_inst);
    }

    /** Sets the playback speed.
      Values greater than 1 mean faster playback; values less than 1 mean
      slower playback.  The default value is 1. 
      <p>
      This does not work for streams in formats other than .cks or Ogg Vorbis. */
    public void setSpeed(float speed)
    {
        nativeSetSpeed(m_inst, speed);
    }

    /** Gets the playback speed.
      Values greater than 1 mean faster playback; values less than 1 mean
      slower playback.  The default value is 1. 
      <p>
      This does not work for streams in formats other than .cks or Ogg Vorbis. */
    public float getSpeed()
    {
        return nativeGetSpeed(m_inst);
    }

    /** Sets the sound to be played when this sound finishes playing.
      The next sound will play immediately, with no gaps. 
      <p>
      This does not work for streams in formats other than .cks or Ogg Vorbis. */
    public void setNextSound(Sound next)
    {
        nativeSetNextSound(m_inst, next == null ? 0 : next.m_inst);
    }

    /** Gets the sound to be played when this sound finishes playing.
      The next sound will play immediately, with no gaps.
      <p>
      This does not work for streams in formats other than .cks or Ogg Vorbis. */
    public Sound getNextSound() 
    {
        return getSound(nativeGetNextSound(m_inst));
    }

    /** Gets the total duration of the sound, in sample frames.
      Stream sounds will return -1 if they are not ready or if the length is unknown. */
    public int getLength()
    {
        return nativeGetLength(m_inst);
    }

    /** Gets the total duration of the sound, in milliseconds.
      Stream sounds will return -1 if they are not ready or if the length is unknown. */
    public float getLengthMs()
    {
        return nativeGetLengthMs(m_inst);
    }

    /** Gets the sample rate of the sound, in Hz.
      Stream sounds will return -1 if they are not ready. */
    public int getSampleRate()
    {
        return nativeGetSampleRate(m_inst);
    }

    /** Gets the number of channels in the sound (1 for mono, 2 for stereo).
      Stream sounds will return -1 if they are not ready. */
    public int getChannels()
    {
        return nativeGetChannels(m_inst);
    }

    /** Sets the effect bus to which this sound's audio is sent to be processed, or
      null for dry output (no effects). 
      <p>
      Effects cannot be applied to streams in formats other than .cks or Ogg Vorbis. */
    public void setEffectBus(EffectBus bus)
    {
        nativeSetEffectBus(m_inst, bus == null ? 0 : bus.m_inst);
    }

    /** Gets the effect bus to which this sound's audio is sent to be processed, or
      null for dry output (no effects).
      <p>
      Effects cannot be applied to streams in formats other than .cks or Ogg Vorbis. */
    EffectBus getEffectBus()
    {
        return EffectBus.getEffectBus(nativeGetEffectBus(m_inst));
    }

    /** Sets whether 3D positioning is enabled.
      If enabled, pan will be determined by the sound and listener positions,
      ignoring the value set by setPan().  Volume will be attenuated based
      on the sound and listener positions and the attenuation settings. */
    public void set3dEnabled(boolean enabled)
    {
        nativeSet3dEnabled(m_inst, enabled);
    }

    /** Gets whether 3D positioning is enabled.
      If enabled, pan will be determined by the sound and listener positions,
      ignoring the value set by setPan().  Volume will be attenuated based
      on the sound and listener positions and the attenuation settings. */
    public boolean is3dEnabled()
    {
        return nativeIs3dEnabled(m_inst);
    }

    /** Gets whether a 3D sound is virtual.
      If, due to distance attenuation, a 3D sound's volume becomes near enough to 0
      to be inaudible, it becomes "virtual".  A virtual sound is effectively paused
      and is not processed until it would become audible again; this reduces the 
      amount of audio processing required. */
    public boolean isVirtual()
    {
        return nativeIsVirtual(m_inst);
    }

    /** Sets the position of the sound emitter in 3D space.
      This is used for pan and volume calculations when 3D positioning is enabled. */
    public void set3dPosition(float x, float y, float z)
    {
        nativeSet3dPosition(m_inst, x, y, z);
    }

    /** Gets the position of the sound emitter in 3D space.
      This is used for pan and volume calculations when 3D positioning is enabled. */
    public void get3dPosition(FloatRef x, FloatRef y, FloatRef z)
    {
        nativeGet3dPosition(m_inst, x, y, z);
    }

    /** Sets the velocity of the sound emitter in 3D space.
      This is used for doppler shift calculations when 3D positioning is enabled. 
      The units should be the same as those used for set3dSoundSpeed(). */
    public void set3dVelocity(float x, float y, float z)
    {
        nativeSet3dVelocity(m_inst, x, y, z);
    }

    /** Gets the velocity of the sound emitter in 3D space.
      This is used for doppler shift calculations when 3D positioning is enabled.
      The units should be the same as those used for set3dSoundSpeed(). */
    public void get3dVelocity(FloatRef x, FloatRef y, FloatRef z)
    {
        nativeGet3dVelocity(m_inst, x, y, z);
    }

    /** Sets the listener position and rotation in 3D space. 
      This is used for pan and volume calculations when 3D positioning is enabled.
      @param eyeX    The listener's position (x coordinate)
      @param eyeY    The listener's position (y coordinate)
      @param eyeZ    The listener's position (z coordinate)
      @param lookAtX The listener's look-at point (x coordinate)
      @param lookAtY The listener's look-at point (y coordinate)
      @param lookAtZ The listener's look-at point (z coordinate)
      @param upX     The listener's up vector (x coordinate)
      @param upY     The listener's up vector (y coordinate)
      @param upZ     The listener's up vector (z coordinate)
      */
    public static void set3dListenerPosition(float eyeX, float eyeY, float eyeZ,
                                             float lookAtX, float lookAtY, float lookAtZ,
                                             float upX, float upY, float upZ)
    {
        nativeSet3dListenerPosition(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);
    }

    /** Gets the listener position and rotation in 3D space. 
      This is used for pan and volume calculations when 3D positioning is enabled.
      @param eyeX    The listener's position (x coordinate)
      @param eyeY    The listener's position (y coordinate)
      @param eyeZ    The listener's position (z coordinate)
      @param lookAtX The listener's look-at point (x coordinate)
      @param lookAtY The listener's look-at point (y coordinate)
      @param lookAtZ The listener's look-at point (z coordinate)
      @param upX     The listener's up vector (x coordinate)
      @param upY     The listener's up vector (y coordinate)
      @param upZ     The listener's up vector (z coordinate)
      */
    public static void get3dListenerPosition(FloatRef eyeX, FloatRef eyeY, FloatRef eyeZ,
                                             FloatRef lookAtX, FloatRef lookAtY, FloatRef lookAtZ,
                                             FloatRef upX, FloatRef upY, FloatRef upZ)
    {
        nativeGet3dListenerPosition(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);
    }

    /** Sets the listener velocity in 3D space.
      This is used for doppler shift calculations when 3D positioning is enabled.
      The units should be the same as those used for set3dSoundSpeed(). */
    public static void set3dListenerVelocity(float x, float y, float z)
    {
        nativeSet3dListenerVelocity(x, y, z);
    }

    /** Gets the listener velocity in 3D space.
      This is used for doppler shift calculations when 3D positioning is enabled.
      The units should be the same as those used for set3dSoundSpeed(). */
    public static void get3dListenerVelocity(FloatRef x, FloatRef y, FloatRef z)
    {
        nativeGet3dListenerVelocity(x, y, z);
    }

    /** Sets parameters used for 3D volume attenuation. 
      @param mode      The attenuation mode
      @param nearDist  The near distance; sounds closer than this distance will not be attenuated.
      @param farDist   The far distance; sounds further than this will be at the far volume.
      @param farVol    The far volume; sounds further than farDist will be at this volume.
      */
    public static void set3dAttenuation(AttenuationMode mode, float nearDist, float farDist, float farVol)
    {
        nativeSet3dAttenuation(mode.value, nearDist, farDist, farVol);
    }

    /** Gets parameters used for 3D volume attenuation. 
      @param mode      The attenuation mode
      @param nearDist  The near distance; sounds closer than this distance will not be attenuated.
      @param farDist   The far distance; sounds further than this will be at the far volume.
      @param farVol    The far volume; sounds further than farDist will be at this volume.
      */
    public static void get3dAttenuation(ObjectRef<AttenuationMode> mode, FloatRef nearDist, FloatRef farDist, FloatRef farVol)
    {
        IntRef modeValue = new IntRef();
        nativeGet3dAttenuation(modeValue, nearDist, farDist, farVol);
        mode.set(AttenuationMode.fromInt(modeValue.get()));
    }

    /** Sets the speed of sound.
      This is used for doppler shift calculations when 3D positioning is enabled.  If no doppler
      shift is desired, this can be set to 0 (which is the default value).
      You may want to use the predefined constants for the speed of sound (such as 
      SOUND_SPEED_METERS_PER_SECOND); use the constant that corresponds to
      the unit system you are using for your velocity and position values.
      You can also exaggerate the doppler effect by using a smaller value. */
    public static void set3dSoundSpeed(float speed)
    {
        nativeSet3dSoundSpeed(speed);
    }

    /** Gets the speed of sound.
      This is used for doppler shift calculations when 3D positioning is enabled.  */
    public static float get3dSoundSpeed()
    {
        return nativeGet3dSoundSpeed();
    }

    /** The speed of sound in dry air at 20 degrees C, in centimeters per second.
      This value is provided as a convenience for set3dSoundSpeed(). */
    public static final float SOUND_SPEED_CENTIMETERS_PER_SECOND = 34320.0f;

    /** The speed of sound in dry air at 20 degrees C, in meters per second.
      This value is provided as a convenience for set3dSoundSpeed(). */
    public static final float SOUND_SPEED_METERS_PER_SECOND = 343.2f;

    /** The speed of sound in dry air at 20 degrees C, in inches per second.
      This value is provided as a convenience for set3dSoundSpeed(). */
    public static final float SOUND_SPEED_INCHES_PER_SECOND = 13512.0f;

    /** The speed of sound in dry air at 20 degrees C, in feet per second.
      This value is provided as a convenience for set3dSoundSpeed(). */
    public static final float SOUND_SPEED_FEET_PER_SECOND = 1126.0f;

    ////////////////////////////////////////

    /** Creates a sound from a bank by index.

      @param bank   The bank
      @param index  Index of the sound in the bank to create
      @return The sound, or null if it could not be created
      */
    public static Sound newBankSound(Bank bank, int index) 
    {
        long inst = nativeNewBankSoundByIndex(bank.m_inst, index);
        return getSound(inst);
    }

    /** Creates a sound from a bank by name.

      @param bank   The bank; if null, all loaded banks will be searched for a sound with a matching name
      @param name   Name of the sound in the bank to create; should be 31 characters or less
      @return The sound, or null if it could not be created
      */
    public static Sound newBankSound(Bank bank, String name)
    {
        long inst = nativeNewBankSoundByName(bank == null ? 0 : bank.m_inst, name);
        return getSound(inst);
    }

    /** Creates a streaming sound from an asset in the .apk.
      Equivalent to newStreamSound(filename, PathType.Asset).

      @param filename   Path to the file to stream.
      @return The sound, or null if it could not be created
      */
    public static Sound newStreamSound(String filename)
    {
        return newStreamSound(filename, PathType.Asset);
    }

    /** Creates a streaming sound.
      Equivalent to newStreamSound(filename, pathType, 0, 0, null).

      @param filename   Path to the file to stream.
      @param pathType   Indicates how the path is to be interpreted.
      @return The sound, or null if it could not be created
      */
    public static Sound newStreamSound(String filename, PathType pathType)
    {
        return newStreamSound(filename, pathType, 0, 0, null);
    }

    /** Creates a streaming sound.

      If the stream file is embedded in a larger file, specify the offset and length of 
      the embedded data in bytes, and provide a string ending with a file extension that indicates
      the file format of the embedded file (for example, ".mp3" or "music.mp3").
      Otherwise, set the offset and length to 0 and the extension to null.

      @param filename   Path to the file to stream.
      @param pathType   Indicates how the path is to be interpreted.
      @param offset     The byte offset of the stream file data in the larger file, or 0 if not embedded.
      @param length     The size of the stream file data in the larger file, or 0 if not embedded.
      @param extension  A string to indicate the file extension of the embedded file (e.g. ".mp3" or "music.mp3"), or null if not embedded.
      @return The sound, or null if it could not be created
      */
    public static Sound newStreamSound(String filename, PathType pathType, int offset, int length, String extension)
    {
        long inst = nativeNewStreamSound(filename, pathType.value, offset, length, extension);
        return getSound(inst);
    }


    ////////////////////////////////////////

    protected void destroyImpl()
    {
        nativeDestroy(m_inst);
    }

    ////////////////////////////////////////

    private Sound(long inst)
    {
        super(inst);
    }

    static Sound getSound(long inst)
    {
        if (inst == 0)
        {
            return null;
        }
        else
        {
            return s_proxies.get(inst);
        }
    }

    static void init()
    {
        s_proxies = new HashMap<Long, Sound>();
    }

    static void shutdown()
    {
        s_proxies = null;
    }

    ////////////////////////////////////////
    // these are called from JNI:

    private static void onNativeCreate(long inst)
    {
        if (s_proxies != null)
        {
            s_proxies.put(inst, new Sound(inst));
        }
    }

    private static void onNativeDestroy(long inst)
    {
        if (s_proxies != null)
        {
            s_proxies.remove(inst);
        }
    }

    ////////////////////////////////////////

    protected static HashMap<Long, Sound> s_proxies;

    private static native void      nativeSetMixer(long inst, long mixerInst);
    private static native long      nativeGetMixer(long inst);
    private static native boolean   nativeIsReady(long inst);
    private static native boolean   nativeIsFailed(long inst);
    private static native void      nativePlay(long inst);
    private static native void      nativeStop(long inst);
    private static native boolean   nativeIsPlaying(long inst);
    private static native void      nativeSetPaused(long inst, boolean paused);
    private static native boolean   nativeIsPaused(long inst);
    private static native boolean   nativeGetMixedPauseState(long inst);
    private static native void      nativeSetLoop(long inst, int loopStart, int loopEnd);
    private static native void      nativeGetLoop(long inst, IntRef loopStart, IntRef loopEnd);
    private static native void      nativeSetLoopCount(long inst, int loopCount);
    private static native int       nativeGetLoopCount(long inst);
    private static native int       nativeGetCurrentLoop(long inst);
    private static native void      nativeReleaseLoop(long inst);
    private static native boolean   nativeIsLoopReleased(long inst);
    private static native void      nativeSetPlayPosition(long inst, int frame);
    private static native void      nativeSetPlayPositionMs(long inst, float ms);
    private static native int       nativeGetPlayPosition(long inst);
    private static native float     nativeGetPlayPositionMs(long inst);
    private static native void      nativeSetVolume(long inst, float volume);
    private static native float     nativeGetVolume(long inst);
    private static native float     nativeGetMixedVolume(long inst);
    private static native void      nativeSetPan(long inst, float pan);
    private static native float     nativeGetPan(long inst);
    private static native void      nativeSetPanMatrix(long inst, float ll, float lr, float rl, float rr);
    private static native void      nativeGetPanMatrix(long inst, FloatRef ll, FloatRef lr, FloatRef rl, FloatRef rr);
    private static native void      nativeSetPitchShift(long inst, float halfSteps);
    private static native float     nativeGetPitchShift(long inst);
    private static native void      nativeSetSpeed(long inst, float speed);
    private static native float     nativeGetSpeed(long inst);
    private static native void      nativeSetNextSound(long inst, long nextInst);
    private static native long      nativeGetNextSound(long inst);
    private static native int       nativeGetLength(long inst);
    private static native float     nativeGetLengthMs(long inst);
    private static native int       nativeGetSampleRate(long inst);
    private static native int       nativeGetChannels(long inst);
    private static native void      nativeSetEffectBus(long inst, long busInst);
    private static native long      nativeGetEffectBus(long inst);
    private static native void      nativeSet3dEnabled(long inst, boolean enabled);
    private static native boolean   nativeIs3dEnabled(long inst);
    private static native boolean   nativeIsVirtual(long inst);
    private static native void      nativeSet3dPosition(long inst, float x, float y, float z);
    private static native void      nativeGet3dPosition(long inst, FloatRef x, FloatRef y, FloatRef z);
    private static native void      nativeSet3dVelocity(long inst, float x, float y, float z);
    private static native void      nativeGet3dVelocity(long inst, FloatRef x, FloatRef y, FloatRef z);
    private static native void      nativeSet3dListenerPosition(float eyeX, float eyeY, float eyeZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ);
    private static native void      nativeGet3dListenerPosition(FloatRef eyeX, FloatRef eyeY, FloatRef eyeZ, FloatRef lookAtX, FloatRef lookAtY, FloatRef lookAtZ, FloatRef upX, FloatRef upY, FloatRef upZ);
    private static native void      nativeSet3dListenerVelocity(float x, float y, float z);
    private static native void      nativeGet3dListenerVelocity(FloatRef x, FloatRef y, FloatRef z);
    private static native void      nativeSet3dAttenuation(int mode, float nearDist, float farDist, float farVol);
    private static native void      nativeGet3dAttenuation(IntRef mode, FloatRef nearDist, FloatRef farDist, FloatRef farVol);
    private static native void      nativeSet3dSoundSpeed(float speed);
    private static native float     nativeGet3dSoundSpeed();

    private static native long      nativeNewBankSoundByIndex(long bankInst, int index);
    private static native long      nativeNewBankSoundByName(long bankInst, String name);
    private static native long      nativeNewStreamSound(String filename, int pathType, int offset, int length, String extension);

    private static native void      nativeDestroy(long inst);
}
