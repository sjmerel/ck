package com.crickettechnology.audio;

abstract class Proxy
{
    /** Destroy this object.
      This is called when the object is finalized by the garbage collector; however, it is better to call it explicitly, so the object is destroyed predictably. */
    public void destroy()
    {
        if (m_inst != 0)
        {
            destroyImpl();
            m_inst = 0;
        }
    }

    protected Proxy(long inst)
    {
        m_inst = inst;
    }

    protected void finalize() throws Throwable
    {
        try
        {
            destroy();
        }
        finally
        {
            super.finalize();
        }
    }

    protected abstract void destroyImpl();

    protected long m_inst;


}

