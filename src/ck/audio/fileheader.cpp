#include "ck/audio/fileheader.h"
#include "ck/core/swapstream.h"

namespace Cki
{

const FourCharCode FileHeader::k_marker('c', 'k', 'm', 'k');

FileHeader::FileHeader() : 
    marker(),
    targets(0),
    fileType(0),
    fileVersion(0)
{
}

SwapStream& operator<<(SwapStream& ser, const FileHeader& value)
{
    // header is always little-endian!
    ByteOrder::Value saved = ser.getByteOrder();
    ser.setByteOrder(ByteOrder::k_littleEndian);

    ser << value.marker;
    ser << value.targets;
    ser << value.fileType;
    ser << value.fileVersion;

    ser.setByteOrder(saved);
    return ser;
}

SwapStream& operator>>(SwapStream& ser, FileHeader& value)
{
    // header is always little-endian!
    ByteOrder::Value saved = ser.getByteOrder();
    ser.setByteOrder(ByteOrder::k_littleEndian);

    ser >> value.marker;
    ser >> value.targets;
    ser >> value.fileType;
    ser >> value.fileVersion;

    ser.setByteOrder(saved);
    return ser;
}


}
