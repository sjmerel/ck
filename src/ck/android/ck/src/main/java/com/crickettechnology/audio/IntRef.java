package com.crickettechnology.audio;

/** Mutable integer container, used for emulating int C++ reference parameters.
  For example, a C++ function 
  <p><code>getSomeValues(int&amp; value1, int&amp; value2)</code></p>
  would be represented in the Java interface as
  <p><code>getSomeValues(IntRef value1, IntRef value2)</code></p>
  */
public final class IntRef
{
    /** Set the value */
    public void set(int value)
    {
        m_value = value;
    }

    /** Get the value */
    public int get()
    {
        return m_value;
    }

    private int m_value;
}


