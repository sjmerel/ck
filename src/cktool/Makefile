CK_TARGET_OS := linux

CK_ROOT := ../..

CK_EXE_NAME := cktool

CK_INCLUDE_DIRS := \
    $(CK_ROOT)/ext/tinyxml

CK_LINK_LIBS := ck tinyxml asound pthread

CK_MAKEFILE_DEPS := \
    $(CK_ROOT)/ext/tinyxml/Makefile \
    $(CK_ROOT)/src/ck/Makefile \

CK_CPP_SRC_FILES := \
    adpcmencoder.cpp \
    aiffreader.cpp \
    argparser.cpp \
    audioreader.cpp \
    encoder.cpp \
    guid.cpp \
    main.cpp \
    pcmi16encoder.cpp \
    pcmi8encoder.cpp \
    wavreader.cpp \

include $(CK_ROOT)/build/Makefile-common
