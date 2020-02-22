CK_ROOT := ../..

CK_LIB_NAME := libck.a

CK_MAKEFILE_DEPS := \
    $(CK_ROOT)/ext/libogg/Makefile \
    $(CK_ROOT)/ext/tremor/Makefile \

CK_INCLUDE_DIRS := \
    $(CK_ROOT)/ext \
    $(CK_ROOT)/ext/libogg/include 

CK_PCH_FILE := core/pch.h

CK_CPP_SRC_FILES := \
    api/bank.cpp \
    api/config.cpp \
    api/customfile.cpp \
    api/effect.cpp \
    api/effectbus.cpp \
    api/effectprocessor.cpp \
    api/mixer.cpp \
    api/sound.cpp \
    api/ck.cpp \
\
    audio/adpcm.cpp \
    audio/adpcmdecoder.cpp \
    audio/audio.cpp \
    audio/audioformat.cpp \
    audio/audiofileinfo.cpp \
    audio/audiograph.cpp \
    audio/audionode.cpp \
    audio/audiosource.cpp \
    audio/audiostream.cpp \
    audio/audioutil.cpp \
    audio/audioutil.neon.cpp \
    audio/audioutil_sse.cpp \
    audio/audiowriter.cpp \
    audio/bank.cpp \
    audio/bankdata.cpp \
    audio/banksound.cpp \
    audio/biquadfilterprocessor.cpp \
    audio/biquadfilterprocessor.neon.cpp \
    audio/biquadfilterprocessor_sse.cpp \
    audio/bitcrusherprocessor.cpp \
    audio/bitcrusherprocessor.neon.cpp \
    audio/bitcrusherprocessor_sse.cpp \
    audio/bufferhandle.cpp \
    audio/bufferpool.cpp \
    audio/buffersource.cpp \
    audio/cksaudiostream.cpp \
    audio/cksstreamsound.cpp \
    audio/commandobject.cpp \
    audio/customaudiostream.cpp \
    audio/customstreamsound.cpp \
    audio/decoder.cpp \
    audio/decoderbuf.cpp \
    audio/distortionprocessor.cpp \
    audio/distortionprocessor.neon.cpp \
    audio/distortionprocessor_sse.cpp \
    audio/effect.cpp \
    audio/effectbus.cpp \
    audio/effectbusnode.cpp \
    audio/effectprocessor.cpp \
    audio/fileheader.cpp \
    audio/fourcharcode.cpp \
    audio/graphoutput.cpp \
    audio/graphoutput_dummy.cpp \
    audio/graphsound.cpp \
    audio/mixer.cpp \
    audio/mixnode.cpp \
    audio/pcmi8decoder.cpp \
    audio/pcmi16decoder.cpp \
    audio/pcmf32decoder.cpp \
    audio/rawwriter.cpp \
    audio/resampler.cpp \
    audio/ringmodprocessor.cpp \
    audio/ringmodprocessor.neon.cpp \
    audio/ringmodprocessor_sse.cpp \
    audio/sample.cpp \
    audio/sampleinfo.cpp \
    audio/sharedbuffer.cpp \
    audio/sound.cpp \
    audio/soundname.cpp \
    audio/sourcenode.cpp \
    audio/streamsound.cpp \
    audio/streamsource.cpp \
    audio/volumematrix.cpp \
    audio/vorbisaudiostream.cpp \
    audio/vorbisstreamsound.cpp \
    audio/wavwriter.cpp \
\
    core/allocatable.cpp \
    core/asyncloader.cpp \
    core/binarystream.cpp \
    core/binarywriter.cpp \
    core/bufferstream.cpp \
    core/byteorder.cpp \
    core/cond.cpp \
    core/customstream.cpp \
    core/debug.cpp \
    core/deletable.cpp \
    core/dir.cpp \
    core/filestream.cpp \
    core/filewriter.cpp \
    core/fixedstring.cpp \
    core/funcallocator.cpp \
    core/glitchfinder.cpp \
    core/lockfreequeue.cpp \
    core/lockingqueue.cpp \
    core/logger.cpp \
    core/math.cpp \
    core/md5.cpp \
    core/mem.cpp \
    core/memoryfixup.cpp \
    core/memorystream.cpp \
    core/mutex.cpp \
    core/path.cpp \
    core/pool.cpp \
    core/profiler.cpp \
    core/readstream.cpp \
    core/ringbuffer.cpp \
    core/runningaverage.cpp \
    core/sse.cpp \
    core/stats.cpp \
    core/string.cpp \
    core/stringwriter.cpp \
    core/substream.cpp \
    core/swapstream.cpp \
    core/system.cpp \
    core/target.cpp \
    core/textwriter.cpp \
    core/thread.cpp \
    core/timer.cpp \
    core/types.cpp \
    core/version.cpp \


include $(CK_ROOT)/build/Makefile-defs


#########################################################
# platform-specific source

ifeq ($(CK_TARGET_OS),linux)
CK_CPP_SRC_FILES += \
    core/timer_linux.cpp \
    core/system_linux.cpp \
    audio/audio_linux.cpp \
    audio/graphoutput_linux.cpp
endif

include $(CK_ROOT)/build/Makefile-common


#########################################################
# ogg vorbis libraries

ogglibs: $(LIB_FILE)
	@echo [appending ogg and tremor libs]
	@$(AR) crs $(LIB_FILE) $(wildcard $(CK_ROOT)/ext/libogg/$(OBJ_DIR)/src/*.o $(CK_ROOT)/ext/tremor/$(OBJ_DIR)/*.o)

all: ogglibs

.PHONY: ogglibs all


#########################################################
# documentation

doc: javadoc doxydoc 
ifeq ($(findstring win,$(HOST_OS)),win)
doc: dotnetdoc
endif

doxydoc:
	@rm -rf $(CK_ROOT)/doc/doxygen
	@cd $(CK_ROOT)/doc/src; doxygen ck.doxyfile

javadoc:
	@rm -rf $(CK_ROOT)/doc/javadoc
	@javadoc -d $(CK_ROOT)/doc/javadoc \
       -public \
       -notimestamp \
       -sourcepath android/ck/src/main/java \
       -classpath $(call fixpath,$(CK_ANDROID_SDK_DIR))/platforms/android-$(CK_API_TARGET)/android.jar \
       -windowtitle "Cricket Audio" \
       com.crickettechnology.audio

dotnetdoc:
	@rm -rf $(CK_ROOT)/doc/dotnet
	@cd $(CK_ROOT)/doc/src; msbuild.exe /p:Configuration=Debug ck.shfbproj


.PHONY: javadoc doxydoc dotnetdoc doc



