#!/bin/bash

cd `dirname $0`  

../../../bin/macosx/cktool buildbank hellocricket.ckbx
#../../../bin/win/vs14.0/x64/DebugDynamic/cktool buildbank hellocricket.ckbx

function copyassets()
{
   mkdir $1
   cp hellocricket.ckb $1
}

copyassets android-sdk/app/src/main/assets
copyassets android-ndk/app/src/main/assets
copyassets android-ndk-native/app/src/main/assets
