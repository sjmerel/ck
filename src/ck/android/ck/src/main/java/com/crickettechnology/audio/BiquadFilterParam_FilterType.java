package com.crickettechnology.audio;

/** Filter types, used for the BiquadFilter.FILTER_TYPE param. */
public final class BiquadFilterParam_FilterType
{
    /** Low pass filter blocks high frequencies. */
    public static final int LOW_PASS = 0;

    /** High pass filter blocks low frequencies. */
    public static final int HIGH_PASS = 1;

    /** Band pass filter blocks frequencies above and below the center frequency. */
    public static final int BAND_PASS = 2;

    /** Notch filter blocks a narrow band of frequencies. */
    public static final int NOTCH = 3;

    /** Peak filter boosts a narrow band of frequencies. */
    public static final int PEAK = 4;

    /** Low shelf filter boosts low frequencies. */
    public static final int LOW_SHELF = 5;

    /** High shelf filter boosts high frequencies. */
    public static final int HIGH_SHELF = 6;

    private BiquadFilterParam_FilterType() {}
}

