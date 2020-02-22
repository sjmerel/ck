#!/bin/bash

cd `dirname $0`  

../../../bin/macosx/cktool buildbank dsptouch.ckbx

mkdir -p android/app/src/main/assets 
cp dsptouch.ckb android/app/src/main/assets

