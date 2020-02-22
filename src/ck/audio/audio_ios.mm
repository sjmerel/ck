#include "ck/audio/audio_ios.h"
#include "ck/audio/audiodebug_ios.h"
#include "ck/audio/audionode.h"
#include "ck/core/debug.h"
#include "ck/core/math.h"
#include "ck/core/logger.h"
#include "ck/core/thread.h"
#include "ck/core/system.h"
#include "ck/core/system_ios.h"

#include <AudioToolbox/AudioToolbox.h>
#include <AVFoundation/AVFoundation.h>
#if CK_PLATFORM_IOS
#  include <MediaPlayer/MediaPlayer.h>
#endif

@class CkiMusicPlayerListener;

namespace
{
    AUGraph g_graph = NULL;
    AUNode g_ioNode = 0;
    AudioUnit g_ioUnit = NULL;
    AUNode g_converterNode = 0;
    AudioUnit g_converterUnit = NULL;

    Float64 g_sampleRate = Cki::Audio::k_outputSampleRate;
}

#if CK_PLATFORM_IOS

////////////////////////////////////////
// When iPod music player starts, it causes an audio session interruption; but when the music player stops, the interruption is not ended.
// This class resumes audio when the music player stops.

namespace
{
    CkiMusicPlayerListener* g_musicPlayerListener = nil;
}

@interface CkiMusicPlayerListener : NSObject
{
    BOOL m_interruption;
    BOOL m_startWhenActive;
}
@end

@implementation CkiMusicPlayerListener

- (id)init 
{
    self = [super init];
    if (self) 
    {
        if (Cki::SystemIos::get()->isVersionGreaterThanOrEqualTo("8.0"))
        {
            [[NSNotificationCenter defaultCenter] 
                addObserver:self 
                selector:@selector(onMusicPlayerPlaybackStateChanged:)
                name:MPMusicPlayerControllerPlaybackStateDidChangeNotification 
                object:[MPMusicPlayerController systemMusicPlayer]
            ];
        }
        if (Cki::SystemIos::get()->isVersionGreaterThanOrEqualTo("6.0"))
        {
            [[NSNotificationCenter defaultCenter] 
                addObserver:self 
                selector:@selector(onAudioSessionInterruption:)
                name:AVAudioSessionInterruptionNotification 
                object:[AVAudioSession sharedInstance]
                ];
            [[NSNotificationCenter defaultCenter] 
                addObserver:self 
                selector:@selector(onAppDidBecomeActive:)
                name:UIApplicationDidBecomeActiveNotification 
                object:nil
                ];
        }
    }
    return self;
}

- (void) dealloc
{
    if (Cki::SystemIos::get()->isVersionGreaterThanOrEqualTo("8.0"))
    {
        [[NSNotificationCenter defaultCenter] removeObserver:self name:MPMusicPlayerControllerPlaybackStateDidChangeNotification object:nil];
    }
    if (Cki::SystemIos::get()->isVersionGreaterThanOrEqualTo("6.0"))
    {
        [[NSNotificationCenter defaultCenter] removeObserver:self name:AVAudioSessionInterruptionNotification object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationDidBecomeActiveNotification object:nil];
    }
    [super dealloc];
}

- (void) stopInterruption
{
    Cki::Audio::stopGraph(); // for some reason, startGraph() on its own won't do it
    Cki::Audio::startGraph();
    if (Cki::SystemIos::get()->isVersionGreaterThanOrEqualTo("8.0"))
    {
        if (m_interruption)
        {
            [[MPMusicPlayerController systemMusicPlayer] endGeneratingPlaybackNotifications];
            m_interruption = NO;
        }
    }
}

- (void) onMusicPlayerPlaybackStateChanged:(NSNotification*) notification
{
    if (m_interruption)
    {
        MPMusicPlaybackState playbackState = [MPMusicPlayerController systemMusicPlayer].playbackState;
        if (playbackState == MPMusicPlaybackStateStopped || playbackState == MPMusicPlaybackStatePaused)
        {
            NSError* err = nil;
            [[AVAudioSession sharedInstance] setActive:YES error:&err];
            CK_ASSERT(err == nil);
            Cki::Audio::stopGraph(); // for some reason, startGraph() on its own won't do it
            Cki::Audio::startGraph();
        }
    }
}

- (void) onAudioSessionInterruption:(NSNotification*) notification
{
    if (g_graph)
    {
        AVAudioSessionInterruptionType interruptionType = [[[notification userInfo] objectForKey:AVAudioSessionInterruptionTypeKey] unsignedIntegerValue];
        if (interruptionType == AVAudioSessionInterruptionTypeBegan)
        {
            CK_LOG_INFO("audio session interruption began");
            if (Cki::SystemIos::get()->isVersionGreaterThanOrEqualTo("8.0"))
            {
                MPMusicPlayerController* player = [MPMusicPlayerController systemMusicPlayer];
                if (player.playbackState == MPMusicPlaybackStatePlaying)
                {
                    if (!m_interruption) 
                    {
                        [player beginGeneratingPlaybackNotifications];
                        m_interruption = YES;
                    }
                }
            }
        }
        else 
        {
            CK_ASSERT(interruptionType == AVAudioSessionInterruptionTypeEnded);
            CK_LOG_INFO("audio session interruption ended");
            NSError* err = nil;
            [[AVAudioSession sharedInstance] setActive:YES error:&err];

            if (err && err.code == AVAudioSessionErrorCodeCannotStartPlaying)
            {
                // if the audio session category doesn't allow background audio, then setActive above will fail when coming
                // back from a phone call, because the app is still in the background; try again when the app comes to the foreground

                m_startWhenActive = YES; 
                /*
                if (Cki::SystemIos::get()->isVersionGreaterThanOrEqualTo("8.0"))
                {
                    m_startWhenActive = YES; 
                }
                else
                {
                    int n = 0;
                    while (err != nil && n++ < 100)
                    {
                        Cki::Thread::sleepMs(50);
                        CK_LOG_INFO("sleeping...");
                        NSError* err = nil;
                        [[AVAudioSession sharedInstance] setActive:YES error:&err];
                    }
                }
                */
            }
            else
            {
                [self stopInterruption];
            }
        }
    }
}

- (void) onAppDidBecomeActive:(NSNotification*) notification
{
    CK_PRINT("became active\n");
    if (m_startWhenActive)
    {
        m_startWhenActive = NO;
        NSError* err = nil;
        [[AVAudioSession sharedInstance] setActive:YES error:&err];
        CK_ASSERT(err == nil);
        [self stopInterruption];
    }
}


@end

#endif // CK_PLATFORM_IOS

////////////////////////////////////////

namespace Cki
{


namespace
{

    ////////////////////////////////////////
    // set/get audio unit properties

    template <typename T>
    void setAudioUnitProperty(AudioUnit unit, AudioUnitPropertyID propertyId, AudioUnitScope scope, AudioUnitElement elem, const T& value)
    {
        CK_OSSTATUS_VERIFY( AudioUnitSetProperty(unit, propertyId, scope, elem, &value, sizeof(value)) );
    }

    template <typename T>
    void getAudioUnitProperty(AudioUnit unit, AudioUnitPropertyID propertyId, AudioUnitScope scope, AudioUnitElement elem, T& value)
    {
        UInt32 size = sizeof(value);
        CK_OSSTATUS_VERIFY( AudioUnitGetProperty(unit, propertyId, scope, elem, &value, &size) );
        CK_ASSERT(size == sizeof(value));
    }


    ////////////////////////////////////////


#if CK_PLATFORM_IOS
    void initSession()
    {
        CkConfig& config = System::get()->getConfig();
        AVAudioSession* session = [AVAudioSession sharedInstance];
        NSError* err = nil;

        // set output sample rate
        // TODO let user choose
        [session setPreferredSampleRate:Audio::k_outputSampleRate error:&err];
        CK_ASSERT(err == nil);

        // set output buffer duration
        Float32 bufferSecs = config.audioUpdateMs * 0.001f;
        [session setPreferredIOBufferDuration:bufferSecs error:&err];
        CK_ASSERT(err == nil);

        [session setActive:NO error:&err];
        [session setActive:YES error:&err];
        CK_ASSERT(err == nil);

        g_sampleRate = session.sampleRate;

        if (!Math::floatEqual((float) g_sampleRate, (float) Audio::k_outputSampleRate, 1.0f))
        {
            CK_LOG_INFO("sample rate is %f (desired %f)", g_sampleRate, (float) Audio::k_outputSampleRate);
        }

#if !TARGET_IPHONE_SIMULATOR
        if ([session respondsToSelector:@selector(IOBufferDuration)]) 
        {
            // available in iOS 6.0
            // simulator seems to ignore this property value, and use a fixed buffer size
            float actualBufferSecs = (float) session.IOBufferDuration;
            CK_LOG_INFO("IO buffer duration is %f ms (desired %f)", actualBufferSecs*1000.0f, bufferSecs*1000.0f);
            config.audioUpdateMs = actualBufferSecs * 1000.0f;
        }
#endif
    }
#endif // CK_PLATFORM_IOS

    void initGraph()
    {
        // create audio graph
        CK_OSSTATUS_VERIFY( NewAUGraph(&g_graph) );


        ////////////////////////////////////////
        // create IO unit
        AudioComponentDescription ioDesc;
        ioDesc.componentType = kAudioUnitType_Output;
#if CK_PLATFORM_IOS || CK_PLATFORM_TVOS
        ioDesc.componentSubType = kAudioUnitSubType_RemoteIO;
#else
        ioDesc.componentSubType = kAudioUnitSubType_DefaultOutput;
#endif
        ioDesc.componentManufacturer = kAudioUnitManufacturer_Apple;
        ioDesc.componentFlags = 0;
        ioDesc.componentFlagsMask = 0;

        CK_OSSTATUS_VERIFY( AUGraphAddNode(g_graph, &ioDesc, &g_ioNode) );


        ////////////////////////////////////////
        // create converter unit
        AudioComponentDescription converterDesc;
        converterDesc.componentType = kAudioUnitType_FormatConverter;
        converterDesc.componentSubType = kAudioUnitSubType_AUConverter;
        converterDesc.componentManufacturer = kAudioUnitManufacturer_Apple;
        converterDesc.componentFlags = 0;
        converterDesc.componentFlagsMask = 0;

        CK_OSSTATUS_VERIFY( AUGraphAddNode(g_graph, &converterDesc, &g_converterNode) );



        // instantiate audio units (not yet initialized)
        CK_OSSTATUS_VERIFY( AUGraphOpen(g_graph) );



        ////////////////////////////////////////
        // configure converter unit
        CK_OSSTATUS_VERIFY( AUGraphNodeInfo(g_graph, g_converterNode, NULL, &g_converterUnit) );

        // set input format
        {
            int bytesPerSample = 4;
            AudioStreamBasicDescription format = {0};
            format.mFormatID = kAudioFormatLinearPCM;
            format.mBytesPerPacket = bytesPerSample * AudioNode::k_maxChannels;
            format.mFramesPerPacket = 1;
            format.mBytesPerFrame = bytesPerSample * AudioNode::k_maxChannels;
            format.mChannelsPerFrame = AudioNode::k_maxChannels;
            format.mBitsPerChannel = 8 * bytesPerSample;
            format.mSampleRate = g_sampleRate;
            if (System::get()->getSampleType() == kCkSampleType_Float)
            {
                format.mFormatFlags = 
                    kAudioFormatFlagIsFloat | 
                    kLinearPCMFormatFlagIsPacked;
            }
            else
            {
                format.mFormatFlags = 
                    kAudioFormatFlagIsSignedInteger | 
                    kLinearPCMFormatFlagIsPacked |
                    (24 << kLinearPCMFormatFlagsSampleFractionShift);

            }
            setAudioUnitProperty(g_converterUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, format);
        }


        CK_OSSTATUS_VERIFY( AUGraphNodeInfo(g_graph, g_ioNode, NULL, &g_ioUnit) );

        // connect 
        CK_OSSTATUS_VERIFY( AUGraphConnectNodeInput(g_graph, g_converterNode, 0, g_ioNode, 0) );

        // init
        CK_OSSTATUS_VERIFY( AUGraphInitialize(g_graph) );

        // start
        CK_OSSTATUS_VERIFY( AUGraphStart(g_graph) );
    }

}

namespace Audio
{

    bool platformInit()
    {
#if CK_PLATFORM_IOS
        initSession();
#endif

#if CK_PLATFORM_OSX || (CK_PLATFORM_IOS && TARGET_IPHONE_SIMULATOR)
        // simulator uses same as mac, regardless of what's set in session property
        System::get()->getConfig().audioUpdateMs = 11.625f;
#endif

        initGraph();

#if CK_PLATFORM_IOS
        g_musicPlayerListener = [[CkiMusicPlayerListener alloc] init];
#endif

        return true;
    }

    void platformShutdown()
    {
        if (g_graph)
        {
            DisposeAUGraph(g_graph);
#if CK_PLATFORM_IOS
            [g_musicPlayerListener release];
#endif
            g_graph = NULL;
        }
    }

    void platformUpdate() {}

    void platformSuspend() {}

    void platformResume() {}

    void setRenderCallback(AURenderCallback callback, void* data)
    {
        // set callback
        AURenderCallbackStruct callbackStruct = { 0 };
        callbackStruct.inputProc = callback;
        callbackStruct.inputProcRefCon = data;
        CK_OSSTATUS_VERIFY( AUGraphSetNodeInputCallback(g_graph, g_converterNode, 0, &callbackStruct) );

        Boolean updated;
        CK_OSSTATUS_VERIFY( AUGraphUpdate(g_graph, &updated) );
    }


    // Start and stop audio graph; note that if the app responds to remote control 
    // events, the play icon in the status bar will show up if the graph is running, and
    // the play/pause button in the "now playing" controls in the multitasking bar will 
    // show the play/pause icon if the graph is stopped/running.
    //
    // Also note that AUGraphStart()/AUGraphStop() appears to keep count of multiple
    // calls, so, for example, if you start twice, you need to stop twice to actually stop it.

    void startGraph()
    {
        Boolean running;
        CK_OSSTATUS_VERIFY( AUGraphIsRunning(g_graph, &running) );
        if (!running)
        {
            CK_OSSTATUS_VERIFY( AUGraphStart(g_graph) );
        }
    }

    void stopGraph()
    {
        Boolean running;
        CK_OSSTATUS_VERIFY( AUGraphIsRunning(g_graph, &running) );
        if (running)
        {
            CK_OSSTATUS_VERIFY( AUGraphStop(g_graph) );
        }
    }

    int getSessionSampleRate()
    {
        return (int) g_sampleRate;
    }

}


}
