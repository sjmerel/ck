#pragma once

#include "ck/core/platform.h"
#include "ck/core/types.h"
#include "ck/audio/filetype.h"
#include "ck/audio/fourcharcode.h"

namespace Cki
{


struct FileHeader
{
    FourCharCode marker;
    uint32 targets;
    uint32 fileType;
    uint32 fileVersion;


    FileHeader();

    static const FourCharCode k_marker;
    enum { k_version = 2 };
};



class SwapStream;

SwapStream& operator<<(SwapStream& ser, const FileHeader& value);
SwapStream& operator>>(SwapStream& ser, FileHeader& value);


}
