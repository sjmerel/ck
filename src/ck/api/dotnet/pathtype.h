#pragma once

#include "ck/pathtype.h"


namespace CricketTechnology
{
namespace Audio
{

/// <summary>
/// Indicates how a file path should be interpreted. 
/// </summary>
public enum class PathType
{
    /// <summary>
    /// A full path to a file on the file system. 
    /// </summary>
    FileSystem = kCkPathType_FileSystem,
        
    /// <summary>
    /// A path relative to the local app data store.
    /// </summary>
    LocalDir = kCkPathType_LocalDir,

    /// <summary>
    /// A path relative to the folder containing your app's package files.
    /// Files in your Visual Studio project with the "Content" property set to true will be installed here.
    /// This folder is read-only.
    /// </summary>
    InstallationDir = kCkPathType_InstallationDir,

    /// <summary>
    /// The default path type for reading files. 
    /// </summary>
    ReadDefault = kCkPathType_ReadDefault,

    /// <summary>
    /// The default path type for writing files. 
    /// </summary>
    WriteDefault = kCkPathType_WriteDefault,

    /// <summary>
    /// The default path type.
    /// </summary>
    Default = kCkPathType_ReadDefault
};


}
}

