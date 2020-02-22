#include "gl.h"
#include "vector3.h"

// Adapted from Mesa3d source (Mesa-7.10.2/src/glu/sgi/libutil/project.c)
// www.mesa3d.org

// The original code is covered by the SGI Free B license:
// http://oss.sgi.com/projects/FreeB/

static void __gluMakeIdentityf(GLfloat m[16])
{
    m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
    m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = 0;
    m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = 0;
    m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

void gluLookAt(
      float eyex, float eyey, float eyez, 
      float centerx, float centery, float centerz, 
      float upx, float upy, float upz)
{
    Vector3 forward, side, up;
    GLfloat m[4][4];

    forward.x = centerx - eyex;
    forward.y = centery - eyey;
    forward.z = centerz - eyez;

    up.x = upx;
    up.y = upy;
    up.z = upz;

    Vector3::normalize(forward, forward);

    /* Side = forward x up */
    Vector3::cross(forward, up, side);
    Vector3::normalize(side, side);

    /* Recompute up as: up = side x forward */
    Vector3::cross(side, forward, up);

    __gluMakeIdentityf(&m[0][0]);
    m[0][0] = side.x;
    m[1][0] = side.y;
    m[2][0] = side.z;

    m[0][1] = up.x;
    m[1][1] = up.y;
    m[2][1] = up.z;

    m[0][2] = -forward.x;
    m[1][2] = -forward.y;
    m[2][2] = -forward.z;

    glMultMatrixf(&m[0][0]);
    glTranslatef(-eyex, -eyey, -eyez);
}
