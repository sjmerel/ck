#!/bin/bash

cd `dirname $0`  

../../../bin/macosx/cktool buildbank game.ckbx
../../../bin/macosx/cktool buildstream music.wav

mkdir -p android/app/src/main/assets
cp game.ckb music.cks android/app/src/main/assets
