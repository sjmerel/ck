package com.crickettechnology.audio;

/** Mutable float container, used for emulating float C++ reference parameters.
  For example, a C++ function 
  <p><code>getSomeValues(float&amp; value1, float&amp; value2)</code></p>
  would be represented in the Java interface as
  <p><code>getSomeValues(FloatRef value1, FloatRef value2)</code></p>
  */
public final class FloatRef
{
    /** Set the value */
    public void set(float value)
    {
        m_value = value;
    }

    /** Get the value */
    public float get()
    {
        return m_value;
    }

    private float m_value;
}

