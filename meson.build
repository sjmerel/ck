project('Cricket', 'c', 'cpp',
        version : '1.6.7',
        default_options : ['warning_level=3', 'c_std=c99', 'cpp_std=c++17'])

incdirs = include_directories('inc', 'src', 'ext', 'ext/libogg/include')

# ogg
src_ogg = ['ext/libogg/src/bitwise.c', 'ext/libogg/src/framing.c']
lib_ogg = static_library('ogg', src_ogg, include_directories : incdirs, name_prefix: '')
objects_ogg = lib_ogg.extract_objects(src_ogg)

# tremor
c_args_tremor = [
	'-DLITTLE_ENDIAN=1', '-DBYTE_ORDER=LITTLE_ENDIAN', '-DHAVE_ALLOCA_H', '-DUSE_MEMORY_H',
	'-Wno-unused-variable', '-Wno-duplicate-decl-specifier'
]
src_tremor = [
	'ext/tremor/block.c', 'ext/tremor/codebook.c', 'ext/tremor/floor0.c', 'ext/tremor/floor1.c', 'ext/tremor/info.c',
	'ext/tremor/mapping0.c', 'ext/tremor/mdct.c', 'ext/tremor/registry.c', 'ext/tremor/res012.c', 'ext/tremor/sharedbook.c',
	'ext/tremor/synthesis.c', 'ext/tremor/vorbisfile.c', 'ext/tremor/window.c'
]
lib_tremor = static_library('tremor', src_tremor, include_directories : incdirs, c_args: c_args_tremor, name_prefix: '')
objects_tremor = lib_tremor.extract_objects(src_tremor)

# Cricket
cpp_args_ck = [
	'-m64', '-msse3', '-mfpmath=sse', '-ffunction-sections', '-funwind-tables', '-fstack-protector',
	'-fno-exceptions', '-fstrict-aliasing', '-fno-rtti',
	'-Wa,--noexecstack', '-Wno-sign-compare', '-Wformat', '-Werror=format-security', '-Werror=strict-aliasing'
]

if get_option('buildtype') == 'release'
	cpp_args_ck += ['-DNDEBUG', '-DCK_NDEBUG=1']
else
	cpp_args_ck += '-DCK_DEBUG=1'
endif

src_ck_core = [
	'src/ck/core/allocatable.cpp', 'src/ck/core/asyncloader.cpp', 'src/ck/core/binarystream.cpp', 'src/ck/core/binarywriter.cpp',
	'src/ck/core/bufferstream.cpp', 'src/ck/core/byteorder.cpp', 'src/ck/core/cond.cpp', 'src/ck/core/customstream.cpp', 'src/ck/core/debug.cpp',
	'src/ck/core/deletable.cpp', 'src/ck/core/dir.cpp', 'src/ck/core/filestream.cpp', 'src/ck/core/filewriter.cpp',
	'src/ck/core/fixedstring.cpp', 'src/ck/core/funcallocator.cpp', 'src/ck/core/glitchfinder.cpp', 'src/ck/core/lockfreequeue.cpp',
	'src/ck/core/lockingqueue.cpp', 'src/ck/core/logger.cpp', 'src/ck/core/math.cpp', 'src/ck/core/md5.cpp', 'src/ck/core/mem.cpp',
	'src/ck/core/memoryfixup.cpp', 'src/ck/core/memorystream.cpp', 'src/ck/core/mutex.cpp', 'src/ck/core/path.cpp', 'src/ck/core/pool.cpp',
	'src/ck/core/profiler.cpp', 'src/ck/core/readstream.cpp', 'src/ck/core/ringbuffer.cpp', 'src/ck/core/runningaverage.cpp',
	'src/ck/core/sse.cpp', 'src/ck/core/stats.cpp', 'src/ck/core/string.cpp', 'src/ck/core/stringwriter.cpp', 'src/ck/core/substream.cpp',
	'src/ck/core/swapstream.cpp', 'src/ck/core/system.cpp', 'src/ck/core/target.cpp', 'src/ck/core/textwriter.cpp', 'src/ck/core/thread.cpp',
	'src/ck/core/timer.cpp', 'src/ck/core/types.cpp', 'src/ck/core/version.cpp'
]

src_ck_audio = [
	'src/ck/audio/adpcm.cpp', 'src/ck/audio/adpcmdecoder.cpp', 'src/ck/audio/audio.cpp', 'src/ck/audio/audioformat.cpp',
	'src/ck/audio/audiofileinfo.cpp', 'src/ck/audio/audiograph.cpp', 'src/ck/audio/audionode.cpp', 'src/ck/audio/audiosource.cpp',
	'src/ck/audio/audiostream.cpp', 'src/ck/audio/audioutil.cpp', 'src/ck/audio/audioutil.neon.cpp', 'src/ck/audio/audioutil_sse.cpp',
	'src/ck/audio/audiowriter.cpp', 'src/ck/audio/bank.cpp', 'src/ck/audio/bankdata.cpp', 'src/ck/audio/banksound.cpp',
	'src/ck/audio/biquadfilterprocessor.cpp', 'src/ck/audio/biquadfilterprocessor.neon.cpp', 'src/ck/audio/biquadfilterprocessor_sse.cpp',
	'src/ck/audio/bitcrusherprocessor.cpp', 'src/ck/audio/bitcrusherprocessor.neon.cpp', 'src/ck/audio/bitcrusherprocessor_sse.cpp',
	'src/ck/audio/bufferhandle.cpp', 'src/ck/audio/bufferpool.cpp', 'src/ck/audio/buffersource.cpp', 'src/ck/audio/cksaudiostream.cpp',
	'src/ck/audio/cksstreamsound.cpp', 'src/ck/audio/commandobject.cpp', 'src/ck/audio/customaudiostream.cpp',
	'src/ck/audio/customstreamsound.cpp', 'src/ck/audio/decoder.cpp', 'src/ck/audio/decoderbuf.cpp', 'src/ck/audio/distortionprocessor.cpp',
	'src/ck/audio/distortionprocessor.neon.cpp', 'src/ck/audio/distortionprocessor_sse.cpp', 'src/ck/audio/effect.cpp',
	'src/ck/audio/effectbus.cpp', 'src/ck/audio/effectbusnode.cpp', 'src/ck/audio/effectprocessor.cpp', 'src/ck/audio/fileheader.cpp',
	'src/ck/audio/fourcharcode.cpp', 'src/ck/audio/graphoutput.cpp', 'src/ck/audio/graphoutput_dummy.cpp', 'src/ck/audio/graphsound.cpp',
	'src/ck/audio/mixer.cpp', 'src/ck/audio/mixnode.cpp', 'src/ck/audio/pcmi8decoder.cpp', 'src/ck/audio/pcmi16decoder.cpp',
	'src/ck/audio/pcmf32decoder.cpp', 'src/ck/audio/rawwriter.cpp', 'src/ck/audio/resampler.cpp', 'src/ck/audio/ringmodprocessor.cpp',
	'src/ck/audio/ringmodprocessor.neon.cpp', 'src/ck/audio/ringmodprocessor_sse.cpp', 'src/ck/audio/sample.cpp',
	'src/ck/audio/sampleinfo.cpp', 'src/ck/audio/sharedbuffer.cpp', 'src/ck/audio/sound.cpp', 'src/ck/audio/soundname.cpp',
	'src/ck/audio/sourcenode.cpp', 'src/ck/audio/streamsound.cpp', 'src/ck/audio/streamsource.cpp', 'src/ck/audio/volumematrix.cpp',
	'src/ck/audio/vorbisaudiostream.cpp', 'src/ck/audio/vorbisstreamsound.cpp', 'src/ck/audio/wavwriter.cpp'
]

src_ck_api = [
	'src/ck/api/bank.cpp', 'src/ck/api/config.cpp', 'src/ck/api/customfile.cpp', 'src/ck/api/effect.cpp', 'src/ck/api/effectbus.cpp',
	'src/ck/api/effectprocessor.cpp', 'src/ck/api/mixer.cpp', 'src/ck/api/sound.cpp', 'src/ck/api/ck.cpp'
]

if build_machine.system() == 'linux'
	src_ck_core += ['src/ck/core/timer_linux.cpp', 'src/ck/core/system_linux.cpp']
	src_ck_audio += ['src/ck/audio/audio_linux.cpp', 'src/ck/audio/graphoutput_linux.cpp']
elif build_machine.system() == 'windows'
	src_ck_core += ['src/ck/core/system_win.cpp', 'src/ck/core/timer_win.cpp']
	src_ck_audio += [
		'src/ck/audio/audio_win.cpp', 'src/ck/audio/audiohelpers_win.cpp', 'src/ck/audio/audiohelpers_win7.cpp',
		'src/ck/audio/audiohelpers_win8.cpp', 'src/ck/audio/graphoutput_win.cpp'
	]
endif

static_library(
	'Cricket', src_ck_core + src_ck_audio + src_ck_api, include_directories : incdirs,
	objects : [objects_ogg, objects_tremor], cpp_args: cpp_args_ck, pic: true, name_prefix: ''
)
