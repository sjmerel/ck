#pragma once

#include <stddef.h>
#include "pathtype.h"


class CkBank;


namespace CricketTechnology
{
namespace Audio
{


/// <summary> 
/// A bank of memory-resident sounds.
/// </summary>
public ref class Bank sealed
{
public:
    /// <summary>
    /// Returns true when an asynchronously loaded bank has finished loading. 
    /// </summary>
    property bool Loaded
    {
        bool get();
    }

    /// <summary>
    /// Returns true if an asynchronously loaded bank had an error during loading. 
    /// </summary>
    property bool Failed
    {
        bool get();
    }

    /// <summary>
    /// Returns the name of the bank. 
    /// </summary>
    property Platform::String^ Name
    {
        Platform::String^ get();
    }

    /// <summary>
    /// Returns the number of sounds in the bank. 
    /// </summary>
    property int NumSounds
    {
        int get();
    }

    /// <summary>
    /// Returns the name of a sound in this bank.
    /// The name will be 31 characters or less. 
    /// </summary>
    Platform::String^ GetSoundName(int index);

    /// <summary>
    /// Loads a bank. 
    /// <para>
    /// Equivalent to NewBank(path, pathType, 0, 0).
    /// </para>
    /// <para>
    /// This may return null if the bank cannot be loaded.
    /// </para>
    /// </summary>
    static Bank^ NewBank(Platform::String^ path, PathType pathType);

    /// <summary>
    /// Loads a bank. 
    /// <para>
    /// Equivalent to NewBank(path, PathType.Default, 0, 0).
    /// </para>
    /// <para>
    /// This may return null if the bank cannot be loaded.
    /// </para>
    /// </summary>
    static Bank^ NewBank(Platform::String^ path);

    /// <summary>
    /// Loads a bank.
    /// <para>
    /// This may return null if the bank cannot be loaded.
    /// </para>
    /// <para>
    /// If the bank file is embedded in a larger file, specify the offset and 
    /// length of the embedded data; otherwise, leave them both as 0.
    /// </para>
    /// </summary>
    static Bank^ NewBank(Platform::String^ path, PathType pathType, int offset, int length);

    /// <summary>
    /// Loads a bank asynchronously. 
    /// <para>
    /// Equivalent to NewBankAsync(path, pathType, 0, 0).
    /// </para>
    /// <para>
    /// This may return null if the bank cannot be loaded.
    /// When loading a bank asynchronously, you should also check isFailed()
    /// after loading to make sure no errors occurred.
    /// </para>
    /// </summary>
    static Bank^ NewBankAsync(Platform::String^ path, PathType pathType);

    /// <summary>
    /// Loads a bank asynchronously. 
    /// <para>
    /// Equivalent to NewBankAsync(path, PathType.Default, 0, 0).
    /// </para>
    /// <para>
    /// This may return null if the bank cannot be loaded.
    /// When loading a bank asynchronously, you should also check isFailed()
    /// after loading to make sure no errors occurred.
    /// </para>
    /// </summary>
    static Bank^ NewBankAsync(Platform::String^ path);

    /// <summary>
    /// Loads a bank asynchronously. 
    /// <para>
    /// This may return null if the bank cannot be loaded.
    /// When loading a bank asynchronously, you should also check isFailed()
    /// after loading to make sure no errors occurred.
    /// </para>
    /// <para>
    /// If the bank file is embedded in a larger file, specify the offset and 
    /// length of the embedded data; otherwise, leave them both as 0.
    /// </para>
    /// </summary>
    static Bank^ NewBankAsync(Platform::String^ path, PathType pathType, int offset, int length);

    /// <summary>
    /// Finds a loaded bank by name; returns null if not found. 
    /// </summary>
    static Bank^ Find(Platform::String^ bankName);

    /// <summary>
    /// Destroys the bank. 
    /// </summary>
    void Destroy();

    virtual ~Bank();

internal:
    Bank(CkBank*);

    void Detach() { m_impl = NULL; }

    property CkBank* Impl
    {
        CkBank* get() { return m_impl; }
    }

private:
    CkBank* m_impl;
};


}
}
