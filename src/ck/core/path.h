#pragma once

#include "ck/core/platform.h"
#include "ck/core/fixedstring.h"
#include "ck/pathtype.h"

namespace Cki
{


class Path
{
public:
    Path();
    Path(const char* path, CkPathType = kCkPathType_FileSystem);

    enum { k_maxLen = 384 }; // should technically be MAXPATHLEN, but that seems overkill (up to 4096)

#if !CK_PLATFORM_WP8
    void setCurrent();
#endif

    void set(const char* path, CkPathType = kCkPathType_FileSystem);
    void append(const char*);
    void appendChild(const char*);

    const char* getBuffer() const;
    int getLength() const;

    int getDepth() const;

    bool isRelative() const;
    bool isAbsolute() const;

    void setParent();

    // get leaf entry: e.g. /a/b/c.txt -> c.txt
    const char* getLeaf() const;

    // get file extension, not including dot, or NULL if none
    const char* getExtension() const;

    // case-insensitive comparison
    bool hasExtension(const char* extension) const;

    // set file extension, not including dot, adding it if there is none; if NULL, will remove any file extension.
    void setExtension(const char* extension);


private:
    typedef FixedString<k_maxLen> PathType;
    PathType m_path; 

    void canonicalize();
};


}
