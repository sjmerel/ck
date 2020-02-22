package com.crickettechnology.audio;

/** Mutable object container, used for emulating C++ reference parameters.
  For example, a C++ function 
  <p><code>getSomeObject(MyClass&amp; obj)</code></p>
  would be represented in the Java interface as
  <p><code>getSomeObject(ObjectRef&lt;MyClass&gt; obj)</code></p>
  */
public final class ObjectRef<T>
{
    /** Set the object */
    public void set(T obj)
    {
        m_obj = obj;
    }

    /** Get the object */
    public T get()
    {
        return m_obj;
    }

    private T m_obj;
}



