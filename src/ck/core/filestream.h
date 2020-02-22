#pragma once

#include "ck/core/platform.h"
#include "ck/core/binarystream.h"

#include <stdio.h>


namespace Cki
{


class FileStream : public BinaryStream
{
public:
    enum Mode
    {
        k_read,             // read-only; file must exist
        k_writeTruncate,    // write-only, and destroy any existing file
        k_readWrite,        // read/write; file must exist
        k_readWriteTruncate,// read/write, and destroy any existing file
    };

    FileStream(const char* path, Mode mode = k_read);
    virtual ~FileStream();

    // BinaryStream overrides
    virtual bool isValid() const;
    virtual int read(void* buf, int bytes);
    virtual int write(const void* buf, int bytes);
    virtual int getSize() const;
    virtual void setPos(int pos);
    virtual int getPos() const;
    virtual void close();

    // Flush file operations to disk
    void flush();



    static bool exists(const char* path);
    static bool destroy(const char* path); // a.k.a. delete
    static int getSize(const char* path); // -1 if file does not exist

private:
    FILE* m_file;
    int m_size;
    int m_pos;

    FileStream(const FileStream&);
    FileStream& operator=(const FileStream&);
};



}
