package com.crickettechnology.audio;

import java.util.HashMap;

/** An audio effect. */
public final class Effect extends Proxy
{
    /** Set an effect parameter. */
    public void setParam(int paramId, float value)
    {
        nativeSetParam(m_inst, paramId, value);
    }

    /** Reset the state of this effect. */
    public void reset()
    {
        nativeReset(m_inst);
    }

    /** Set whether this effect is bypassed.
      A bypassed effect is not applied to the audio. */
    public void setBypassed(boolean bypassed)
    {
        nativeSetBypassed(m_inst, bypassed);
    }

    /** Gets whether this effect is bypassed.
      A bypassed effect is not applied to the audio. */
    public boolean isBypassed()
    {
        return nativeIsBypassed(m_inst);
    }

    /** Sets the wet/dry ratio for this effect.
      The ratio can range from 0 to 1.
      A value of 0 means the audio is left completely "dry", that is, with no
      effects applied; it is equivalent to bypassing the effect.
      A value of 1 means the audio is completely "wet", that is, the original
      "dry" signal is not mixed back into the processed audio. */
    public void setWetDryRatio(float wetDry)
    {
        nativeSetWetDryRatio(m_inst, wetDry);
    }

    /** Gets the wet/dry ratio for this effect.
      The ratio can range from 0 to 1.
      A value of 0 means the audio is left completely "dry", that is, with no
      effects applied; it is equivalent to bypassing the effect.
      A value of 1 means the audio is completely "wet", that is, the original
      "dry" signal is not mixed back into the processed audio. */
    public float getWetDryRatio()
    {
        return nativeGetWetDryRatio(m_inst);
    }

    /** Create a built-in effect. */
    public static Effect newEffect(EffectType type)
    {
        long inst = nativeNewEffect(type.value);
        return getEffect(inst);
    }

    /** Create a custom effect (previously registered with the C++ API). */
    public static Effect newCustomEffect(int id)
    {
        long inst = nativeNewCustomEffect(id);
        return getEffect(inst);
    }

    ////////////////////////////////////////

    protected void destroyImpl()
    {
        nativeDestroy(m_inst);
    }

    ////////////////////////////////////////

    private Effect(long inst)
    {
        super(inst);
    }

    static Effect getEffect(long inst)
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
        s_proxies = new HashMap<Long, Effect>();
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
            s_proxies.put(inst, new Effect(inst));
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

    protected static HashMap<Long, Effect> s_proxies;

    private static native void      nativeSetParam(long inst, int paramId, float value);
    private static native void      nativeReset(long inst);
    private static native void      nativeSetBypassed(long inst, boolean bypass);
    private static native boolean   nativeIsBypassed(long inst);
    private static native void      nativeSetWetDryRatio(long inst, float wetDry);
    private static native float     nativeGetWetDryRatio(long inst);

    private static native long      nativeNewEffect(int type);
    private static native long      nativeNewCustomEffect(int id);

    private static native void      nativeDestroy(long inst);
}


