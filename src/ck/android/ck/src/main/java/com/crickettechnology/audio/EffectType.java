package com.crickettechnology.audio;

/** Types of built-in effects available. */
public enum EffectType
{
    /** Biquadratic filter. 
     See BiquadFilterParam values for parameter IDs. */
    BIQUAD_FILTER(0),

    /** Bit Crusher effect reduces bit resolution and/or bit rate, producing a retro
      low-fi 8-bit sound.
     See BitCrusherParam values for parameter IDs. */
    BIT_CRUSHER(1),

    /** Ring Mod effect modulates the audio by a sine wave.
     See RingModParam values for parameter IDs. */
    RING_MOD(2),

    /** Distortion effect.
     See DistortionParam values for parameter IDs. */
    DISTORTION(3);

    ////////////////////////////////////////

    private EffectType(int value) { this.value = value; }
    final int value;

    private EffectType() { this.value = 0; }
}


