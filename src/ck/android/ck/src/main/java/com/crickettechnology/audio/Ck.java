package com.crickettechnology.audio;

import android.content.Context;

/** The Cricket Audio interface. */
public final class Ck
{
    /** Initializes Cricket Audio. 
      @param context  The context that will be playing sounds
     */
    public static void init(Context context)
    {
        init(context, null);
    }

    /** Initializes Cricket Audio. 
      @param context  The context that will be playing sounds
      @param config  Configuration info (can be null)
     */
    public static void init(Context context, Config config)
    {
        if (!s_inited)
        {
            if (config == null)
            {
                config = new Config(); // use defaults
            }

            try
            {
                System.loadLibrary("ckjava");
            }
            catch (UnsatisfiedLinkError ex)
            {
                android.util.Log.e("CK", "Could not load library libckjava.so; make sure it is in the libs/armeabi directory of your project.");
                throw ex;
            }

            Bank.init();
            Effect.init();
            EffectBus.init();
            Mixer.init();
            Sound.init();

            nativeInit(context, config);
            s_inited = true;
        }
    }

    /** Shuts down Cricket Audio. */
    public static void shutdown()
    {
        if (s_inited)
        {
            Bank.shutdown();
            Effect.shutdown();
            EffectBus.shutdown();
            Mixer.shutdown();
            Sound.shutdown();

            // TODO unload library? need to create our own classloader to do so.
            nativeShutdown();
            s_inited = false;
        }
    }

    /** Updates Cricket Audio.
      Should be called once per frame.
      */
    public static void update()
    {
        nativeUpdate();
    }

    /** Suspends Cricket Audio. 
      Typically used when an app is made inactive. 
     */
    public static void suspend()
    {
        nativeSuspend();
    }

    /** Resumes Cricket Audio. 
      Typically used when an app is made active after having been inactive. */
    public static void resume()
    {
        nativeResume();
    }

    /** Returns an estimate of the load on the audio processing thread.
      This is the fraction of the interval between audio callbacks that was actually spent
      rendering audio; it will range between 0 and 1. */
    public static float getRenderLoad()
    {
        return nativeGetRenderLoad();
    }

    /** Returns the value of the clip flag.
      The clip flag is set to true whenever the final audio output "clips", i.e. 
      exceeds the maximum value. */
    public static boolean getClipFlag()
    {
        return nativeGetClipFlag();
    }

    /** Resets the value of the clip flag to false.  
      The clip flag is set to true whenever the final audio output "clips", i.e. 
      exceeds the maximum value. */
    public static void resetClipFlag()
    {
        nativeResetClipFlag();
    }

    /** Sets the maximum rate at which volumes change on playing sounds, in ms per full volume scale. 
      Default value is 40 ms. */
    public static void setVolumeRampTime(float ms)
    {
        nativeSetVolumeRampTime(ms);
    }

    /** Gets the maximum rate at which volumes change on playing sounds, in ms per full volume scale. */
    public static float getVolumeRampTime()
    {
        return nativeGetVolumeRampTime();
    }

    /** Lock the audio processing thread to prevent it from processing any API calls.
      The lock should be held only for a short time.  This is useful to ensure that
      certain calls are processed together.  For example, if you want to play multiple
      sounds together exactly in sync, call lockAudio(), call play() on each sound,
      then call unlockAudio(). */
    public static void lockAudio()
    {
        nativeLockAudio();
    }

    /** Unlock the audio processing thread after a call to lockAudio(). */
    public static void unlockAudio()
    {
        nativeUnlockAudio();
    }

    /** Start capturing the final audio output to a file.  
      The filename must end with ".wav" (for a Microsoft WAVE file) or ".raw" 
      (for a headerless file containing 32-bit floating-point interleaved stereo samples). */
    public static void startCapture(String path, PathType pathType)
    {
        nativeStartCapture(path, pathType.value);
    }

    /** Stop capturing audio output. */
    public static void stopCapture()
    {
        nativeStopCapture();
    }

    ////////////////////////////////////////

    private Ck() {}

    private static boolean s_inited = false;
    private static native void nativeInit(Context context, Config config);
    private static native void nativeShutdown();
    private static native void nativeSuspend();
    private static native void nativeResume();
    private static native void nativeUpdate();
    private static native float nativeGetRenderLoad();
    private static native boolean nativeGetClipFlag();
    private static native void nativeResetClipFlag();
    private static native void nativeSetVolumeRampTime(float ms);
    private static native float nativeGetVolumeRampTime();
    private static native void nativeLockAudio();
    private static native void nativeUnlockAudio();
    private static native void nativeStartCapture(String path, int pathType);
    private static native void nativeStopCapture();
}


