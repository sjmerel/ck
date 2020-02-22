package com.crickettechnology.audio;

import java.util.HashMap;

/** Manages a set of audio effects applied to sounds. */
public final class EffectBus extends Proxy
{
    /** Add an effect to this bus.
      An effect can only be on one bus at a time. */
    public void addEffect(Effect effect)
    {
        nativeAddEffect(m_inst, effect.m_inst);
    }

    /** Remove an effect from this bus. */
    public void removeEffect(Effect effect)
    {
        nativeRemoveEffect(m_inst, effect.m_inst);
    }

    /** Remove all effects from this bus. */
    public void removeAllEffects()
    {
        nativeRemoveAllEffects(m_inst);
    }

    /** Set the bus to which this bus's output is sent.
      If null (the default), the audio from this bus is mixed into the final output. */
    public void setOutputBus(EffectBus outputBus)
    {
        nativeSetOutputBus(m_inst, (outputBus == null ? 0 : outputBus.m_inst));
    }

    /** Get the bus to which this bus's output is sent.
      If null (the default), the audio from this bus is mixed into the final output. */
    public EffectBus getOutputBus()
    {
        return getEffectBus(nativeGetOutputBus(m_inst));
    }

    /** Reset the state of all effects on this bus. */
    public void reset()
    {
        nativeReset(m_inst);
    }

    /** Set whether this bus is bypassed.
      A bypassed effect is not applied to the audio. */
    public void setBypassed(boolean bypassed)
    {
        nativeSetBypassed(m_inst, bypassed);
    }

    /** Gets whether this bus is bypassed.
      Effects in a bypassed bus are not applied to the audio. */
    public boolean isBypassed()
    {
        return nativeIsBypassed(m_inst);
    }

    /** Sets the wet/dry ratio for this bus.
      The ratio can range from 0 to 1.
      A value of 0 means the audio is left completely "dry", that is, with no
      effects applied; it is equivalent to bypassing the bus.
      A value of 1 means the audio is completely "wet", that is, the original
      "dry" signal is not mixed back into the processed audio. */
    public void setWetDryRatio(float wetDry)
    {
        nativeSetWetDryRatio(m_inst, wetDry);
    }

    /** Gets the wet/dry ratio for this bus.
      The ratio can range from 0 to 1.
      A value of 0 means the audio is left completely "dry", that is, with no
      effects applied; it is equivalent to bypassing the bus.
      A value of 1 means the audio is completely "wet", that is, the original
      "dry" signal is not mixed back into the processed audio. */
    public float getWetDryRatio()
    {
        return nativeGetWetDryRatio(m_inst);
    }

    ////////////////////////////////////////

    /** Create a new effect bus. */
    public static EffectBus newEffectBus()
    {
        long inst = nativeNewEffectBus();
        return getEffectBus(inst);
    }

    /** Get the global effect bus.
      Effects on this bus are applied to the final audio before it is output. */
    public static EffectBus getGlobalEffectBus()
    {
        return getEffectBus(nativeGetGlobalEffectBus());
    }


    ////////////////////////////////////////

    protected void destroyImpl()
    {
        nativeDestroy(m_inst);
    }

    ////////////////////////////////////////

    private EffectBus(long inst)
    {
        super(inst);
    }

    static EffectBus getEffectBus(long inst)
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
        s_proxies = new HashMap<Long, EffectBus>();
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
            s_proxies.put(inst, new EffectBus(inst));
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

    protected static HashMap<Long, EffectBus> s_proxies;

    private static native void      nativeAddEffect(long inst, long effectInst);
    private static native void      nativeRemoveEffect(long inst, long effectInst);
    private static native void      nativeRemoveAllEffects(long inst);
    private static native void      nativeSetOutputBus(long inst, long outputBusInst);
    private static native long      nativeGetOutputBus(long inst);
    private static native void      nativeReset(long inst);
    private static native void      nativeSetBypassed(long inst, boolean bypass);
    private static native boolean   nativeIsBypassed(long inst);
    private static native void      nativeSetWetDryRatio(long inst, float wetDry);
    private static native float     nativeGetWetDryRatio(long inst);

    private static native long      nativeNewEffectBus();
    private static native long      nativeGetGlobalEffectBus();

    private static native void      nativeDestroy(long inst);
}

