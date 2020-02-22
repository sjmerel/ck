# copy to local dir

dstdir="../../lib/${PLATFORM_NAME}/${CONFIGURATION}"
mkdir -p $dstdir 
cp ${BUILT_PRODUCTS_DIR}/${EXECUTABLE_NAME} ${dstdir}
