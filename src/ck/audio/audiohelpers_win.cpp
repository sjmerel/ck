#if CK_PLATFORM_WIN
#   ifndef CK_WINVER
#      error "this file should not be compiled directly, but only included through audiohelpers_win7.cpp or audiohelpers_win8.cpp"
#   endif
#endif

#include "ck/audio/audiohelpers_win.h"
#include "ck/audio/audiodebug_win.h"
#include "ck/audio/audionode.h"
#include "ck/core/logger.h"

// on Windows, include path is different for audiohelpers_win7 and audiohelpers_win8, so each
// includes a different xaudio2 header.
// (Make sure xaudio2 is not included in pch.h!)
#include <xaudio2.h> 

namespace Cki
{


namespace CK_CONCAT_VALUE(AudioHelpers, CK_WINVER)
{
    IXAudio2* g_xAudio2 = NULL;
    IXAudio2MasteringVoice* g_masterVoice = NULL;
    IXAudio2SourceVoice* g_sourceVoice = NULL;
    HMODULE g_module = NULL;

    BufferEndCallback g_bufferEndCb = NULL;
    void* g_bufferEndCbData = NULL;

    class CallbackObj : public IXAudio2VoiceCallback
    {
        virtual void __stdcall OnStreamEnd() {}
        virtual void __stdcall OnVoiceProcessingPassEnd() {}
        virtual void __stdcall OnVoiceProcessingPassStart(UINT32) {}
        virtual void __stdcall OnBufferEnd(void*)
        {
            g_bufferEndCb(g_bufferEndCbData);
        }
        virtual void __stdcall OnBufferStart(void*) {}
        virtual void __stdcall OnLoopEnd(void*) {}
        virtual void __stdcall OnVoiceError(void*, HRESULT) {}
    };

    CallbackObj g_callbackObj;

    bool init()
    {
        bool success = true;

        CoInitializeEx(NULL, COINIT_MULTITHREADED);

#if CK_PLATFORM_WIN
#  if CK_WINVER == 7
        CK_LOG_INFO("Loading xaudio 2.7\n");
        g_module = LoadLibrary("xaudio2_7.dll");
#  else
        CK_LOG_INFO("Loading xaudio 2.8\n");
        g_module = LoadLibrary("xaudio2_8.dll");
#  endif

        if (!g_module)
        {
            CK_LOG_ERROR("Could not load XAudio2 library");
            return false;
        }
#endif

        HRESULT result = NULL;
#if CK_PLATFORM_WIN
#  if CK_WINVER == 7
        UINT32 flags = 0;
#    if CK_DEBUG 
//        flags = XAUDIO2_DEBUG_ENGINE;
#    endif
        result = XAudio2Create(&g_xAudio2, flags, XAUDIO2_DEFAULT_PROCESSOR);
#  else // 8
        typedef HRESULT (__stdcall *XAudioCreateFunc)(_Outptr_ IXAudio2**, UINT32, XAUDIO2_PROCESSOR);
        XAudioCreateFunc createFunc = (XAudioCreateFunc) GetProcAddress(g_module, "XAudio2Create");
        result = createFunc(&g_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
#  endif
#else // wp8
        result = XAudio2Create(&g_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
#endif

        if (FAILED(result))
        {
            CK_LOG_ERROR("Could not initialize XAudio2 (error %x); is it installed?", result);
            success = false;
        }

        if (g_xAudio2)
        {
            if (FAILED(g_xAudio2->CreateMasteringVoice(&g_masterVoice, 2, 48000, 0, 0, NULL)))
            {
                success = false;
            }
        }

        return success;
    }

    void shutdown()
    {
        if (g_masterVoice)
        {
            g_masterVoice->DestroyVoice();
        }
        if (g_xAudio2)
        {
            g_xAudio2->Release();
            g_xAudio2 = NULL;
        }
        if (g_module)
        {
            FreeLibrary(g_module);
        }
    }

    void createVoice(BufferEndCallback cb, void* data)
    {
        CK_ASSERT(!g_sourceVoice);

        const int k_sampleBytes = sizeof(float);

        WAVEFORMATEX format = { 0 };
        format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
        format.nChannels = AudioNode::k_maxChannels;
        format.nSamplesPerSec = AudioNode::k_maxSampleRate;
        format.nAvgBytesPerSec = AudioNode::k_maxSampleRate * AudioNode::k_maxChannels * k_sampleBytes;
        format.nBlockAlign = AudioNode::k_maxChannels * k_sampleBytes;
        format.wBitsPerSample = k_sampleBytes * 8;

        g_bufferEndCb = cb;
        g_bufferEndCbData = data;

        CK_HRESULT_VERIFY( g_xAudio2->CreateSourceVoice(&g_sourceVoice, &format, 0, 2.0f, &g_callbackObj, NULL, NULL) );
    }

    void destroyVoice()
    {
        g_sourceVoice->DestroyVoice();
        g_bufferEndCb = NULL;
        g_bufferEndCbData = NULL;
    }

    void startVoice()
    {
        CK_HRESULT_VERIFY( g_sourceVoice->Start(0) );
    }

    void stopVoice()
    {
        CK_HRESULT_VERIFY( g_sourceVoice->Stop(0) );
        CK_HRESULT_VERIFY( g_sourceVoice->FlushSourceBuffers() );
    }

    void submitVoiceBuffer(float* buf, int bufSize)
    {
        XAUDIO2_BUFFER buffer = { 0 };
        buffer.AudioBytes = bufSize;
        buffer.pAudioData = (BYTE*) buf;

        CK_HRESULT_VERIFY( g_sourceVoice->SubmitSourceBuffer(&buffer) );
    }
}


}

