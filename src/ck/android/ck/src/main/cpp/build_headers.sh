#!/bin/bash 

cd `dirname $0`

for class in Bank Ck Config Effect EffectBus Mixer Sound
do
   echo generating header for $class
   javah -classpath ../../../../lib/android/sdk/ck.jar com.crickettechnology.audio.$class
done


for hfile in *.h
do
   hdefs=`fgrep Java_ $hfile | wc -l`

   cfile=`basename $hfile .h`.cpp
   cdefs=`fgrep Java_ $cfile | wc -l`

   if [ $hdefs -ne $cdefs ]
   then
      echo $cfile missing definition? 
      echo " $hdefs definitions in .h"
      echo " $cdefs definitions in .c"
   fi
done
