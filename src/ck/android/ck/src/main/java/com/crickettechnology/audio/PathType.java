package com.crickettechnology.audio;

/** Indicates how a file path should be interpreted. */
public class PathType
{
    private PathType(int _value) { value = _value; }
    final int value;

    /** A full path to a file on the file system. */
    public static final PathType FileSystem = new PathType(0);

    /** A path to an asset in the .apk. */
    public static final PathType Asset = new PathType(1);

    /** A path relative to the private files directory. */
    public static final PathType PrivateFiles = new PathType(2);

    /** A path relative to the external storage directory. */
    public static final PathType ExternalStorage = new PathType(3);

    /** The default path type for reading files. */
    public static final PathType ReadDefault = Asset;

    /** The default path type for writing files. */
    public static final PathType WriteDefault = PrivateFiles;

    /** The default path type. */
    public static final PathType Default = ReadDefault;
}
