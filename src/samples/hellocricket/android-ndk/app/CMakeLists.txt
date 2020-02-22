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
    hellocricket
    SHARED

    src/main/cpp/hellocricket.cpp
    )

target_link_libraries( 
    hellocricket

    ck
    cpufeatures
    log
    )





