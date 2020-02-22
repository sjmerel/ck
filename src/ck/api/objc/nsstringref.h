#pragma once
#import <Foundation/Foundation.h>

class NsStringRef
{
public:
    NsStringRef(NSString*);
    ~NsStringRef();

    const char* getChars();

    enum { k_len = 256 };

private:
    NSString* m_string;
    char m_buf[k_len];
};

