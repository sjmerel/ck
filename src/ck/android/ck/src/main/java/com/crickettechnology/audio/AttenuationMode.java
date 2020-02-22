package com.crickettechnology.audio;

/** Indicates how volume on 3D sounds is attenuated with distance. */
public class AttenuationMode
{
    private AttenuationMode(int _value) { value = _value; }
    final int value;

    /** No attenuation; volume is 1.0 when closer than the far distance, and far volume outside of that. */
    public static final AttenuationMode None = new AttenuationMode(0);

    /** Linear attenuation of volume between near distance and far distance. */
    public static final AttenuationMode Linear = new AttenuationMode(1);

    /** Attenuation proportional to the reciprocal of the distance. */
    public static final AttenuationMode InvDistance = new AttenuationMode(2);

    /** Attenuation proportional to the square of the reciprocal of the distance. 
     This is a good choice for realistically simulating point sound sources. */
    public static final AttenuationMode InvDistanceSquared = new AttenuationMode(3);

    static AttenuationMode fromInt(int value)
    {
        if (value == None.value)
            return None;
        else if (value == Linear.value)
            return Linear;
        else if (value == InvDistance.value)
            return InvDistance;
        else if (value == InvDistanceSquared.value)
            return InvDistanceSquared;
        else
            return null;
    }
}

