#pragma once
#include "ck/platform.h"

#if CK_PLATFORM_ANDROID
#  include <GLES/gl.h>
#elif CK_PLATFORM_OSX || CK_PLATFORM_IOS
#  include <OpenGLES/ES1/gl.h>
#endif

// eye: position of eye
// center: position of center of scene that eye is looking at
// up: up vector
void gluLookAt(
    float eyex, float eyey, float eyez, 
    float centerx, float centery, float centerz, 
    float upx, float upy, float upz);
