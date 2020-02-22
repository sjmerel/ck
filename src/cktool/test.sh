#!/bin/bash

set -o nounset
set -o errexit

function onExit()
{
   if [ $? != 0 ] 
   then 
      echo "EXITED WITH ERRORS!" 
   fi
}
trap onExit  EXIT


cd `dirname $0`  

rm -f test.log


function runTest()
{
   $1 buildbank ../test/srcassets/test.ckbx test.ckb >> test.log || exit 1
   $1 info test.ckb >> test.log || exit 1
   rm test.ckb

   $1 buildbank -single ../test/srcassets/benfolds.wav benfolds.ckb >> test.log || exit 1
   $1 info benfolds.ckb >> test.log || exit 1
   rm benfolds.ckb

   $1 buildbank -single -format adpcm -volume 0.2 -pan -0.2 -loopStart 3 -loopEnd 1000 -loopCount 3 ../test/srcassets/benfolds.wav benfolds.ckb >> test.log || exit 1
   $1 info benfolds.ckb >> test.log || exit 1
   rm benfolds.ckb

   $1 buildstream ../test/srcassets/benfolds.wav benfolds.cks >> test.log || exit 1
   $1 info benfolds.cks >> test.log || exit 1
   rm benfolds.cks

   $1 buildstream -format adpcm -volume 0.2 -pan -0.2 -loopStart 3 -loopEnd 1000 -loopCount 3 ../test/srcassets/benfolds.wav benfolds.cks >> test.log || exit 1
   $1 info benfolds.cks >> test.log || exit 1
   rm benfolds.cks
}


if [[ "$OSTYPE" =~ "darwin" ]]
then
   runTest ../../bin/macosx/cktool
elif [[ "$OSTYPE" =~ "linux-gnu" ]]
then
   for platform in x86 x64
   do
       runTest ../../bin/linux/$platform/release/cktool
   done
else
   for platform in x86 x64
   do
       runTest ../../bin/win/vs11.0/$platform/ReleaseStatic/cktool.exe
   done
fi

