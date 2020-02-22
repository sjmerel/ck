cmake_minimum_required(VERSION 3.4.1)
set( CK_ROOT ../../../../.. )

include(AndroidNdkModules)
android_ndk_import_module_cpufeatures()

include_directories(
    ${CK_ROOT}/inc
    )

link_directories(
    ${CK_ROOT}/lib/android/ndk/${ANDROID_ABI}/debug
    )


add_library(
    game
    SHARED

    src/main/cpp/bindings.cpp
    ../../cube.cpp
    ../../game.cpp
    ../../gl.cpp
    ../../groundplane.cpp
    ../../ship.cpp
    ../../timer.cpp
    )

target_link_libraries( 
    game

    ck
    GLESv1_CM
    cpufeatures
    log
    )





