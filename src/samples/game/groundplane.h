#pragma once
#include "gl.h"
#include <vector>
#include "vector3.h"

class GroundPlane
{
public:
    GroundPlane();
    ~GroundPlane();

    // init/destroy texture; GL context must exist.
    void initGraphics();
    void destroyGraphics();

    void draw();

private:
    std::vector<Vector3> m_verts;
    std::vector<GLubyte> m_faceInds;
    std::vector<float> m_texCoords;
    GLuint m_tex;
};



