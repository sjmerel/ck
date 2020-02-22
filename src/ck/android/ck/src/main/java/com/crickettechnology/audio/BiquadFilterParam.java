package com.crickettechnology.audio;

/** Parameters for the Biquad Filter effect */
public final class BiquadFilterParam
{
    /** Type of filter; should be one of the BiquadFilterType values. */
    public static final int FILTER_TYPE = 0;

    /** Center frequency of the filter. */
    public static final int FREQ = 1;

    /** Q of the filter. */
    public static final int Q = 2;

    /** Gain in dB (for PEAK, LOW_SHELF, and HIGH_SHELF filter types) */
    public static final int GAIN = 3;

    private BiquadFilterParam() {}
}

