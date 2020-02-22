# combine iOS architectures

outdir="../../lib/ios/${CONFIGURATION}"
if [ ! -e $outdir ]
then
   mkdir -p $outdir
fi

armlib="${BUILD_DIR}/${CONFIGURATION}-iphoneos/${EXECUTABLE_NAME}"
simlib="${BUILD_DIR}/${CONFIGURATION}-iphonesimulator/${EXECUTABLE_NAME}"
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

xcrun -sdk iphoneos lipo $inlibs -create -output $outlib

