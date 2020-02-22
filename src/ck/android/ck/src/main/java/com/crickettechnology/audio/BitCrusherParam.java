package com.crickettechnology.audio;

/** Parameters for the Bit Crusher effect */
public final class BitCrusherParam
{
    /** Number of bits of resolution to keep in the input samples, in [1..24]. */
    public static final int BIT_RESOLUTION = 0;

    /** Milliseconds to hold each sample value.  Higher values result in more
      reduction in the effective sample rate.  Values around 1 ms provide a reasonable effect. */
    public static final int HOLD_MS = 1;

    private BitCrusherParam() {}
}

