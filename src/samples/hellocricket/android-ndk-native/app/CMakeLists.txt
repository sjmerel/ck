cmake_minimum_required(VERSION 3.4.1)
set( CK_ROOT ../../../../.. )

include(AndroidNdkModules)
android_ndk_import_module_cpufeatures()
android_ndk_import_module_native_app_glue()

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
    native_app_glue
    log
    android
    )





