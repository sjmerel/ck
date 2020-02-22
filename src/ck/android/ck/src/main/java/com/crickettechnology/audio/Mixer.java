package com.crickettechnology.audio;

import java.util.HashMap;

/** A mixer represents a hierarchical grouping of sounds for controlling volumes. */
public class Mixer extends Proxy
{
    /** Sets the name of the mixer.
      @param name   The name; should be 31 characters or less. */
    public void setName(String name)
    {
        nativeSetName(m_inst, name);
    }

    /** Returns the name of the mixer.
      The name will be 31 characters or less. */
    public String getName()
    {
        return nativeGetName(m_inst);
    }

    /** Set the volume of this mixer.
      (The actual volume used during mixing will also depend on ancestors.) */
    public void setVolume(float volume)
    {
        nativeSetVolume(m_inst, volume);
    }

    /** Get the volume of this mixer.
      (The actual volume used during mixing will also depend on ancestors.) */
    public float getVolume()
    {
        return nativeGetVolume(m_inst);
    }

    /** Returns the volume used for mixing (affected by ancestors). */
    public float getMixedVolume()
    {
        return nativeGetMixedVolume(m_inst);
    }

    /** Set whether this mixer is paused.
      A sound will be paused if the sound, its mixer, or any of its mixer's ancestors
      are paused. */
    public void setPaused(boolean paused)
    {
        nativeSetPaused(m_inst, paused);
    }

    /** Get whether this mixer is paused.
      A sound will be paused if the sound, its mixer, or any of its mixer's ancestors
      are paused. */
    public boolean isPaused()
    {
        return nativeIsPaused(m_inst);
    }

    /** Returns true if this mixer, or any of its ancestors, is paused. */
    public boolean getMixedPauseState()
    {
        return nativeGetMixedPauseState(m_inst);
    }

    /** Sets the parent of the mixer.
      Setting to null sets the master mixer as the parent. */
    public void setParent(Mixer parent)
    {
        nativeSetParent(m_inst, (parent == null ? 0 : parent.m_inst));
    }

    /** Returns the parent of the mixer.
      The return value is null only for the master mixer. */
    public Mixer getParent()
    {
        return getMixer(nativeGetParent(m_inst));
    }



    ////////////////////////////////////////

    /** Returns the master mixer, which is the root of the mixer hierarchy. */
    public static Mixer getMaster()
    {
        return getMixer(nativeGetMaster());
    }

    /** Creates a new mixer.
      The initial volume of the mixer is 1.0.
      @param name   The name of the mixer; should be 31 characters or less
      @param parent The mixer's parent; if null, parent will be the master mixer */
    public static Mixer newMixer(String name, Mixer parent)
    {
        long inst = nativeNewMixer(name, parent == null ? 0 : parent.m_inst);
        return getMixer(inst);
    }

    /** Creates a new mixer.
      The mixer is parented to the master mixer, and its initial volume is 1.0.
      @param name   The name of the mixer; should be 31 characters or less */
    public static Mixer newMixer(String name)
    {
        return newMixer(name, null);
    }

    /** Finds a mixer by name.
      @param name The mixer name; should be 31 characters or less
      @return The mixer, or null if none with that name */
    public static Mixer find(String name)
    {
        return getMixer(nativeFind(name));
    }


    ////////////////////////////////////////

    protected void destroyImpl() 
    { 
        nativeDestroy(m_inst); 
    }

    ////////////////////////////////////////

    private Mixer(long inst)
    {
        super(inst);
    }

    static Mixer getMixer(long inst)
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
        s_proxies = new HashMap<Long, Mixer>();
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
            s_proxies.put(inst, new Mixer(inst));
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

    private static HashMap<Long, Mixer> s_proxies;

    private static native void      nativeSetName(long inst, String name);
    private static native String    nativeGetName(long inst);
    private static native void      nativeSetVolume(long inst, float volume);
    private static native float     nativeGetVolume(long inst);
    private static native float     nativeGetMixedVolume(long inst);
    private static native void      nativeSetPaused(long inst, boolean paused);
    private static native boolean   nativeIsPaused(long inst);
    private static native boolean   nativeGetMixedPauseState(long inst);
    private static native void      nativeSetParent(long inst, long parent);
    private static native long      nativeGetParent(long inst);

    private static native long      nativeGetMaster();
    private static native long      nativeNewMixer(String name, long parent);
    private static native long      nativeFind(String name);

    private static native void      nativeDestroy(long inst);
}

