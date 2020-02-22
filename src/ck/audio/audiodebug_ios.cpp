#include "ck/audio/audiodebug_ios.h"
#include <CoreAudio/CoreAudioTypes.h>

namespace Cki
{


namespace Audio
{


    void printASBD(const AudioStreamBasicDescription& format)
    {
        if (format.mFormatID == kAudioFormatLinearPCM)
        {
            // print friendly description of output format
            // e.g.:
            // "44.1 kHz, 16-bit signed integer, big-endian interleaved stereo LPCM"
            CK_PRINT("%.1f kHz, ", format.mSampleRate * 0.001f);
            bool isFloat = (format.mFormatFlags & kAudioFormatFlagIsFloat) != 0;
            bool isBigEndian = (format.mFormatFlags & kAudioFormatFlagIsBigEndian) != 0;
            const char* endianStr = isBigEndian ? "big-endian" : "little-endian";
            if (isFloat)
            {
                CK_PRINT("%d-bit %s float, ", (int) format.mBitsPerChannel, endianStr);
            }
            else
            {
                int sampleFraction = (format.mFormatFlags & kLinearPCMFormatFlagsSampleFractionMask) >> kLinearPCMFormatFlagsSampleFractionShift;
                if (sampleFraction)
                {
                    CK_PRINT("%d.%d ", (int) format.mBitsPerChannel - sampleFraction, sampleFraction);
                }
                else
                {
                    CK_PRINT("%d-bit ", (int) format.mBitsPerChannel);
                }
                bool isSigned = (format.mFormatFlags & kAudioFormatFlagIsSignedInteger) != 0;
                CK_PRINT("%s %s integer, ", isSigned ? "signed" : "unsigned", endianStr);
            }

            bool isInterleaved = (format.mFormatFlags & kAudioFormatFlagIsNonInterleaved) == 0;
            if (isInterleaved)
            {
                CK_PRINT("interleaved ");
            }
            else
            {
                CK_PRINT("non-interleaved ");
            }

            if (format.mChannelsPerFrame == 1)
            {
                CK_PRINT("mono ");
            }
            else if (format.mChannelsPerFrame == 2)
            {
                CK_PRINT("stereo ");
            }
            else 
            {
                CK_PRINT("%d-channel ", (int) format.mChannelsPerFrame);
            }

            CK_PRINT("LPCM\n");
        }
        else
        {
            // print generic description
            CK_PRINT("sample rate: %f\n", format.mSampleRate);
            CK_PRINT("format id: %c%c%c%c\n", 
                    (int)((format.mFormatID & 0xff000000) >> 24),
                    (int)((format.mFormatID & 0x00ff0000) >> 16),
                    (int)((format.mFormatID & 0x0000ff00) >> 8),
                    (int)((format.mFormatID & 0x000000ff) >> 0));
            CK_PRINT("bytes per packet: %u\n", (uint) format.mBytesPerPacket);
            CK_PRINT("frames per packet: %u\n", (uint) format.mFramesPerPacket);
            CK_PRINT("bytes per frame: %u\n", (uint) format.mBytesPerFrame);
            CK_PRINT("channels per frame: %u\n", (uint) format.mChannelsPerFrame);
            CK_PRINT("bits per channel: %u\n", (uint) format.mBitsPerChannel);
            CK_PRINT("flags:\n");
            CK_PRINT(" IsFloat: %u\n", (uint) ((format.mFormatFlags & kAudioFormatFlagIsFloat) != 0));
            CK_PRINT(" IsBigEndian: %u\n", (uint) ((format.mFormatFlags & kAudioFormatFlagIsBigEndian) != 0));
            CK_PRINT(" IsSignedInteger: %u\n", (uint) ((format.mFormatFlags & kAudioFormatFlagIsSignedInteger) != 0));
            CK_PRINT(" IsPacked: %u\n", (uint) ((format.mFormatFlags & kAudioFormatFlagIsPacked) != 0));
            CK_PRINT(" IsAlignedHigh: %u\n", (uint) ((format.mFormatFlags & kAudioFormatFlagIsAlignedHigh) != 0));
            CK_PRINT(" IsNonInterleaved: %u\n", (uint) ((format.mFormatFlags & kAudioFormatFlagIsNonInterleaved) != 0));
            CK_PRINT(" IsNonMixable: %u\n", (uint) ((format.mFormatFlags & kAudioFormatFlagIsNonMixable) != 0));
        }
    }
}

}
