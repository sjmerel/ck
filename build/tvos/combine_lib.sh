# combine tvOS architectures

outdir="../../lib/tvos/${CONFIGURATION}"
if [ ! -e $outdir ]
then
   mkdir -p $outdir
fi

armlib="${BUILD_DIR}/${CONFIGURATION}-appletvos/${EXECUTABLE_NAME}"
simlib="${BUILD_DIR}/${CONFIGURATION}-appletvsimulator/${EXECUTABLE_NAME}"
outlib="$outdir/${EXECUTABLE_NAME}"

inlibs=""
if [ -e "$armlib" ] 
then 
   inlibs="$inlibs $armlib"
fi
if [ -e "$simlib" ] 
then 
   inlibs="$inlibs $simlib"
fi

xcrun -sdk appletvos lipo $inlibs -create -output $outlib


