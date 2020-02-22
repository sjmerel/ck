package com.crickettechnology.audio;

import java.util.HashMap;

/** A bank of memory-resident sounds. */
public final class Bank extends Proxy
{

    /** Returns true when an asynchronously loaded bank has finished loading. */
    public boolean isLoaded()
    {
        return nativeIsLoaded(m_inst);
    }

    /** Returns true if an asynchronously loaded bank had an error during loading. */
    public boolean isFailed()
    {
        return nativeIsFailed(m_inst);
    }

    /** Returns the name of the bank. */
    public String getName()
    {
        return nativeGetName(m_inst);
    }

    /** Returns the number of sounds in the bank. */
    public int getNumSounds()
    {
        return nativeGetNumSounds(m_inst);
    }

    /** Returns the name of a sound in this bank.
      The name will be 31 characters or less. */
    public String getSoundName(int index)
    {
        return nativeGetSoundName(m_inst, index);
    }


    ////////////////////////////////////////

    /** Loads a bank from an asset in the .apk.
      Equivalent to newBank(filename, PathType.Asset, 0, 0).

      This may return NULL if the bank cannot be loaded.

      @param filename   Path to the bank file (.ckb).
      @return The bank, or null if it could not be loaded
      */
    public static Bank newBank(String filename)
    {
        return newBank(filename, PathType.Asset);
    }

    /** Loads a bank.
      Equivalent to newBank(filename, pathType, 0, 0).

      This may return NULL if the bank cannot be loaded.

      @param filename   Path to the bank file (.ckb).
      @param pathType   Indicates how the path is to be interpreted.
      @return The bank, or null if it could not be loaded
      */
    public static Bank newBank(String filename, PathType pathType)
    {
        return newBank(filename, pathType, 0, 0);
    }

    /** Loads a bank.

      This may return NULL if the bank cannot be loaded.

      If the bank file is embedded in a larger file, specify the offset and 
      length of the embedded data; otherwise, set them both to 0.

      @param filename   Path to the bank file (.ckb).
      @param pathType   Indicates how the path is to be interpreted.
      @param offset     The byte offset of the bank file data in the larger file, or 0 if not embedded.
      @param length     The size of the bank file data in the larger file, or 0 if not embedded.
      @return The bank, or null if it could not be loaded
      */
    public static Bank newBank(String filename, PathType pathType, int offset, int length)
    {
        long inst = nativeNewBank(filename, pathType.value, offset, length);
        return getBank(inst);
    }

    /** Loads a bank asynchronously from an asset in the .apk.
      Equivalent to newBankAsync(filename, PathType.Asset, 0, 0).

      This may return NULL if the bank cannot be loaded.
      When loading a bank asynchronously, you should also check isFailed()
      after loading to make sure no errors occurred.

      @param filename   Path to the bank file (.ckb).
      @return The bank, or null if it could not be loaded
      */
    public static Bank newBankAsync(String filename)
    {
        return newBankAsync(filename, PathType.Asset);
    }

    /** Loads a bank asynchronously.
      Equivalent to newBankAsync(filename, pathType, 0, 0);

      This may return NULL if the bank cannot be loaded.
      When loading a bank asynchronously, you should also check isFailed()
      after loading to make sure no errors occurred.

      @param filename   Path to the bank file (.ckb).
      @param pathType   Indicates how the path is to be interpreted.
      @return The bank, or null if it could not be loaded
      */
    public static Bank newBankAsync(String filename, PathType pathType)
    {
        return newBankAsync(filename, pathType, 0, 0);
    }

    /** Loads a bank asynchronously.

      This may return NULL if the bank cannot be loaded.
      When loading a bank asynchronously, you should also check isFailed()
      after loading to make sure no errors occurred.

      If the bank file is embedded in a larger file, specify the offset and 
      length of the embedded data; otherwise, leave them both as 0.

      @param filename   Path to the bank file (.ckb).
      @param pathType   Indicates how the path is to be interpreted.
      @param offset     The byte offset of the bank file data in the larger file.
      @param length     The size of the bank file data in the larger file.
      @return The bank, or null if it could not be loaded
      */
    public static Bank newBankAsync(String filename, PathType pathType, int offset, int length)
    {
        long inst = nativeNewBankAsync(filename, pathType.value, offset, length);
        return getBank(inst);
    }


    /** Find a loaded bank by name.
      @param name The bank name; should be 31 characters or less
      @return The bank, or null if none with that name 
      */
    public static Bank find(String name)
    {
        return getBank(nativeFind(name));
    }

    // TODO loadFromMemory


    ////////////////////////////////////////

    protected void destroyImpl() 
    { 
        nativeDestroy(m_inst); 
    }


    ////////////////////////////////////////

    private Bank(long inst) 
    {
        super(inst);
    }

    static Bank getBank(long inst)
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
        s_proxies = new HashMap<Long, Bank>();
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
            s_proxies.put(inst, new Bank(inst));
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

    protected static HashMap<Long, Bank> s_proxies;

    private static native boolean   nativeIsLoaded(long inst);
    private static native boolean   nativeIsFailed(long inst);
    private static native String    nativeGetName(long inst);
    private static native int       nativeGetNumSounds(long inst);
    private static native String    nativeGetSoundName(long inst, int index);

    private static native long      nativeNewBank(String filename, int pathType, int offset, int length);
    private static native long      nativeNewBankAsync(String filename, int pathType, int offset, int length);
    private static native long      nativeFind(String name);

    private static native void      nativeDestroy(long inst);

}


