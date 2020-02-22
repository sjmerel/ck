cmake_minimum_required(VERSION 3.4.1)
set( CK_ROOT ../../../../ )

include(AndroidNdkModules)
android_ndk_import_module_cpufeatures()


if (${CMAKE_BUILD_TYPE} EQUAL "Debug")
    add_definitions( -DCK_DEBUG=1 )
else()
    add_definitions( -DCK_NDEBUG=1 )
endif()

STRING(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)



include_directories(
    ${CK_ROOT}/src
    ${CK_ROOT}/inc
    ${CK_ROOT}/ext
    ${CK_ROOT}/ext/libogg/include
    )


########################################
#libogg

add_library(
    libogg
    OBJECT

    ${CK_ROOT}/ext/libogg/src/bitwise.c
    ${CK_ROOT}/ext/libogg/src/framing.c
    )


########################################
# tremor

add_library(
    tremor
    OBJECT

    # TODO -Wno-duplicate-decl-specifier
    ${CK_ROOT}/ext/tremor/block.c
    ${CK_ROOT}/ext/tremor/codebook.c
    ${CK_ROOT}/ext/tremor/floor0.c
    ${CK_ROOT}/ext/tremor/floor1.c
    ${CK_ROOT}/ext/tremor/info.c
    ${CK_ROOT}/ext/tremor/mapping0.c
    ${CK_ROOT}/ext/tremor/mdct.c
    ${CK_ROOT}/ext/tremor/registry.c
    ${CK_ROOT}/ext/tremor/res012.c
    ${CK_ROOT}/ext/tremor/sharedbook.c
    ${CK_ROOT}/ext/tremor/synthesis.c
    ${CK_ROOT}/ext/tremor/vorbisfile.c
    ${CK_ROOT}/ext/tremor/window.c 
    )
target_compile_options(
    tremor 
    PRIVATE

    "-Wno-duplicate-decl-specifier"
    "-Wno-absolute-value"
    )

########################################

set(CK_NEON_SRC
    ${CK_ROOT}/src/ck/audio/audioutil.neon.cpp
    ${CK_ROOT}/src/ck/audio/biquadfilterprocessor.neon.cpp
    ${CK_ROOT}/src/ck/audio/bitcrusherprocessor.neon.cpp
    ${CK_ROOT}/src/ck/audio/distortionprocessor.neon.cpp
    ${CK_ROOT}/src/ck/audio/ringmodprocessor.neon.cpp
    )

set(CK_SSE_SRC
    ${CK_ROOT}/src/ck/audio/audioutil_sse.cpp
    ${CK_ROOT}/src/ck/audio/biquadfilterprocessor_sse.cpp
    ${CK_ROOT}/src/ck/audio/bitcrusherprocessor_sse.cpp
    ${CK_ROOT}/src/ck/audio/distortionprocessor_sse.cpp
    ${CK_ROOT}/src/ck/audio/ringmodprocessor_sse.cpp
    )

if (${ANDROID_ABI} STREQUAL "armeabi-v7a")
    set_property(SOURCE ${CK_NEON_SRC}
        APPEND_STRING PROPERTY COMPILE_FLAGS " -mfpu=neon")
elseif (${ANDROID_ABI} STREQUAL "x86")
    set_property(SOURCE ${CK_SSE_SRC}
        APPEND_STRING PROPERTY COMPILE_FLAGS " -mssse3")
endif ()

add_library( 
    ck
    STATIC

    ${CK_ROOT}/src/ck/api/bank.cpp
    ${CK_ROOT}/src/ck/api/config.cpp
    ${CK_ROOT}/src/ck/api/customfile.cpp
    ${CK_ROOT}/src/ck/api/effect.cpp
    ${CK_ROOT}/src/ck/api/effectbus.cpp
    ${CK_ROOT}/src/ck/api/effectprocessor.cpp
    ${CK_ROOT}/src/ck/api/mixer.cpp
    ${CK_ROOT}/src/ck/api/sound.cpp
    ${CK_ROOT}/src/ck/api/ck.cpp

    ${CK_ROOT}/src/ck/audio/adpcm.cpp
    ${CK_ROOT}/src/ck/audio/adpcmdecoder.cpp
    ${CK_ROOT}/src/ck/audio/audio.cpp
    ${CK_ROOT}/src/ck/audio/audioformat.cpp
    ${CK_ROOT}/src/ck/audio/audiofileinfo.cpp
    ${CK_ROOT}/src/ck/audio/audiograph.cpp
    ${CK_ROOT}/src/ck/audio/audionode.cpp
    ${CK_ROOT}/src/ck/audio/audiosource.cpp
    ${CK_ROOT}/src/ck/audio/audiostream.cpp
    ${CK_ROOT}/src/ck/audio/audioutil.cpp
    ${CK_ROOT}/src/ck/audio/audiowriter.cpp
    ${CK_ROOT}/src/ck/audio/bank.cpp
    ${CK_ROOT}/src/ck/audio/bankdata.cpp
    ${CK_ROOT}/src/ck/audio/banksound.cpp
    ${CK_ROOT}/src/ck/audio/biquadfilterprocessor.cpp
    ${CK_ROOT}/src/ck/audio/bitcrusherprocessor.cpp
    ${CK_ROOT}/src/ck/audio/bufferhandle.cpp
    ${CK_ROOT}/src/ck/audio/bufferpool.cpp
    ${CK_ROOT}/src/ck/audio/buffersource.cpp
    ${CK_ROOT}/src/ck/audio/cksaudiostream.cpp
    ${CK_ROOT}/src/ck/audio/cksstreamsound.cpp
    ${CK_ROOT}/src/ck/audio/commandobject.cpp
    ${CK_ROOT}/src/ck/audio/customaudiostream.cpp
    ${CK_ROOT}/src/ck/audio/customstreamsound.cpp
    ${CK_ROOT}/src/ck/audio/decoder.cpp
    ${CK_ROOT}/src/ck/audio/decoderbuf.cpp
    ${CK_ROOT}/src/ck/audio/distortionprocessor.cpp
    ${CK_ROOT}/src/ck/audio/effect.cpp
    ${CK_ROOT}/src/ck/audio/effectbus.cpp
    ${CK_ROOT}/src/ck/audio/effectbusnode.cpp
    ${CK_ROOT}/src/ck/audio/effectprocessor.cpp
    ${CK_ROOT}/src/ck/audio/fileheader.cpp
    ${CK_ROOT}/src/ck/audio/fourcharcode.cpp
    ${CK_ROOT}/src/ck/audio/graphoutput.cpp
    ${CK_ROOT}/src/ck/audio/graphoutput_dummy.cpp
    ${CK_ROOT}/src/ck/audio/graphsound.cpp
    ${CK_ROOT}/src/ck/audio/mixer.cpp
    ${CK_ROOT}/src/ck/audio/mixnode.cpp
    ${CK_ROOT}/src/ck/audio/pcmi8decoder.cpp
    ${CK_ROOT}/src/ck/audio/pcmi16decoder.cpp
    ${CK_ROOT}/src/ck/audio/pcmf32decoder.cpp
    ${CK_ROOT}/src/ck/audio/rawwriter.cpp
    ${CK_ROOT}/src/ck/audio/resampler.cpp
    ${CK_ROOT}/src/ck/audio/ringmodprocessor.cpp
    ${CK_ROOT}/src/ck/audio/sample.cpp
    ${CK_ROOT}/src/ck/audio/sampleinfo.cpp
    ${CK_ROOT}/src/ck/audio/sharedbuffer.cpp
    ${CK_ROOT}/src/ck/audio/sound.cpp
    ${CK_ROOT}/src/ck/audio/soundname.cpp
    ${CK_ROOT}/src/ck/audio/sourcenode.cpp
    ${CK_ROOT}/src/ck/audio/streamsound.cpp
    ${CK_ROOT}/src/ck/audio/streamsource.cpp
    ${CK_ROOT}/src/ck/audio/volumematrix.cpp
    ${CK_ROOT}/src/ck/audio/vorbisaudiostream.cpp
    ${CK_ROOT}/src/ck/audio/vorbisstreamsound.cpp
    ${CK_ROOT}/src/ck/audio/wavwriter.cpp

    ${CK_ROOT}/src/ck/core/allocatable.cpp
    ${CK_ROOT}/src/ck/core/asyncloader.cpp
    ${CK_ROOT}/src/ck/core/binarystream.cpp
    ${CK_ROOT}/src/ck/core/binarywriter.cpp
    ${CK_ROOT}/src/ck/core/bufferstream.cpp
    ${CK_ROOT}/src/ck/core/byteorder.cpp
    ${CK_ROOT}/src/ck/core/cond.cpp
    ${CK_ROOT}/src/ck/core/customstream.cpp
    ${CK_ROOT}/src/ck/core/debug.cpp
    ${CK_ROOT}/src/ck/core/deletable.cpp
    ${CK_ROOT}/src/ck/core/dir.cpp
    ${CK_ROOT}/src/ck/core/filestream.cpp
    ${CK_ROOT}/src/ck/core/filewriter.cpp
    ${CK_ROOT}/src/ck/core/fixedstring.cpp
    ${CK_ROOT}/src/ck/core/funcallocator.cpp
    ${CK_ROOT}/src/ck/core/glitchfinder.cpp
    ${CK_ROOT}/src/ck/core/lockfreequeue.cpp
    ${CK_ROOT}/src/ck/core/lockingqueue.cpp
    ${CK_ROOT}/src/ck/core/logger.cpp
    ${CK_ROOT}/src/ck/core/math.cpp
    ${CK_ROOT}/src/ck/core/md5.cpp
    ${CK_ROOT}/src/ck/core/mem.cpp
    ${CK_ROOT}/src/ck/core/memoryfixup.cpp
    ${CK_ROOT}/src/ck/core/memorystream.cpp
    ${CK_ROOT}/src/ck/core/mutex.cpp
    ${CK_ROOT}/src/ck/core/path.cpp
    ${CK_ROOT}/src/ck/core/pool.cpp
    ${CK_ROOT}/src/ck/core/profiler.cpp
    ${CK_ROOT}/src/ck/core/readstream.cpp
    ${CK_ROOT}/src/ck/core/ringbuffer.cpp
    ${CK_ROOT}/src/ck/core/runningaverage.cpp
    ${CK_ROOT}/src/ck/core/sse.cpp
    ${CK_ROOT}/src/ck/core/stats.cpp
    ${CK_ROOT}/src/ck/core/string.cpp
    ${CK_ROOT}/src/ck/core/stringwriter.cpp
    ${CK_ROOT}/src/ck/core/substream.cpp
    ${CK_ROOT}/src/ck/core/swapstream.cpp
    ${CK_ROOT}/src/ck/core/system.cpp
    ${CK_ROOT}/src/ck/core/target.cpp
    ${CK_ROOT}/src/ck/core/textwriter.cpp
    ${CK_ROOT}/src/ck/core/thread.cpp
    ${CK_ROOT}/src/ck/core/timer.cpp
    ${CK_ROOT}/src/ck/core/types.cpp
    ${CK_ROOT}/src/ck/core/version.cpp

    # android-specific
    ${CK_ROOT}/src/ck/audio/audio_android.cpp
    ${CK_ROOT}/src/ck/audio/audiodebug_android.cpp
    ${CK_ROOT}/src/ck/audio/audiotrackproxy_android.cpp
    ${CK_ROOT}/src/ck/audio/graphoutput_android.cpp
    ${CK_ROOT}/src/ck/audio/graphoutputjava_android.cpp
    ${CK_ROOT}/src/ck/audio/nativestreamsound_android.cpp
    ${CK_ROOT}/src/ck/audio/opensles_android.cpp
    ${CK_ROOT}/src/ck/core/assetmanager_android.cpp
    ${CK_ROOT}/src/ck/core/assetstream_android.cpp
    ${CK_ROOT}/src/ck/core/jnienv_android.cpp
    ${CK_ROOT}/src/ck/core/system_android.cpp
    ${CK_ROOT}/src/ck/core/timer_linux.cpp 

    ${CK_NEON_SRC}
    ${CK_SSE_SRC}

    $<TARGET_OBJECTS:libogg> 
    $<TARGET_OBJECTS:tremor> 
    )

set_target_properties(
    ck 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/android/ndk/${ANDROID_ABI}/${CMAKE_BUILD_TYPE_LOWER}
    )


add_library(
    ckjava
    SHARED

    # JNI
    src/main/cpp/com_crickettechnology_audio_Bank.cpp
    src/main/cpp/com_crickettechnology_audio_Ck.cpp
    src/main/cpp/com_crickettechnology_audio_Effect.cpp
    src/main/cpp/com_crickettechnology_audio_EffectBus.cpp
    src/main/cpp/com_crickettechnology_audio_Mixer.cpp
    src/main/cpp/com_crickettechnology_audio_Sound.cpp
    src/main/cpp/intref.cpp
    src/main/cpp/floatref.cpp 
    )

target_link_libraries( 
    ckjava

    ck
    cpufeatures
    log
    )


