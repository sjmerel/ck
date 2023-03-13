# combine 32-bit and 64-bit architectures

outdir="$1/macosx/${CONFIGURATION}"
if [ ! -e $outdir ]
then
   mkdir -p $outdir
fi

infile="${BUILD_DIR}/${CONFIGURATION}/${EXECUTABLE_NAME}"
outfile="$outdir/${EXECUTABLE_NAME}"

if [ "$ARCHS" == "i386" ]
then
   otherarch="x86_64"
else
   otherarch="i386"
fi

if [ ! -e $outfile ]
then
   echo lipo $infile -create -output $outfile
   lipo $infile -create -output $outfile
else
   echo lipo $outfile -replace $ARCHS $infile -output $outfile
   lipo $outfile -replace $ARCHS $infile -output $outfile
fi


cp $outfile $outdir/..
