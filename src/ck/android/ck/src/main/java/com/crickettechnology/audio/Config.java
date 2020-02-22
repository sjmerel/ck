package com.crickettechnology.audio;

/** Configuration options for the Engine. */
public final class Config
{
    public static final int LOG_INFO = (1 << 0);
    public static final int LOG_WARNING = (1 << 1);
    public static final int LOG_ERROR = (1 << 2);

    public static final int LOG_NONE = 0;
    public static final int LOG_ALL = (LOG_INFO | LOG_WARNING | LOG_ERROR);


    /** If true, always use Java audio output instead of OpenSL ES.
      This should usually be set to false, unless you are debugging specific issues
      related to low-level audio output.
      Default is false (use Java audio output only when OpenSL ES is not available, i.e. Android versions prior to 2.3).*/
    public boolean UseJavaAudio = false;

    /** If true, software support for Ogg Vorbis stream sounds is enabled.
      If false, stream sounds will use the native Android decoder, which will not
      support all of the features available to sounds decoded in software.
      Default is true (Ogg Vorbis streams are decoded in software). */
    public boolean EnableOggVorbis = true;

    // TODO allocFunc, freeFunc?
    // TODO logFunc?

    /** Bitmask for which types of messages are logged. 
      Default is LOG_ALL (all messages logged). */
    public int LogMask = LOG_ALL;

    /** Audio processing frame duration, in ms.
      Shorter times result in lower latency, but require more CPU.
      Default is 5 ms. */
    public float AudioUpdateMs = 5.0f;

    /** Size of buffer for stream sounds.
      Larger buffers require more memory, but are less likely
      to skip due to slow file reads.
      Default is 500 ms. */
    public float StreamBufferMs = 500.0f;

    /** Interval between file reads for stream sounds, in ms.
      Shorter times will allow for smaller stream buffer sizes, but require more CPU.
      This value should be set to significantly less than streamBufferMs, or your buffers will always underrun!
      Default is 100 ms. */
    public float StreamFileUpdateMs = 100.0f;

    /** Maximum number of audio tasks that can be queued at once.  Increase this
      from its default value of 500 if you see a warning stating "Couldn't produce
      task for audio graph thread". */
    public int MaxAudioTasks = 500;

    /** Maximum render load. When the value of Ck.getRenderLoad() exceeds this value,
      some sounds will not be rendered, to keep the CPU usage down.
      Default is 0.8. */
    public float MaxRenderLoad = 0.8f;

}
