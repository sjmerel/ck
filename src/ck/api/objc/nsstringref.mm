#import "ck/api/objc/nsstringref.h"

NsStringRef::NsStringRef(NSString* str) :
    m_string(str)
{
    [m_string retain];
    [m_string getCString:m_buf maxLength:k_len encoding:NSUTF8StringEncoding];
}

NsStringRef::~NsStringRef()
{
    [m_string release];
}

const char* NsStringRef::getChars()
{
    return m_buf;
}
