#include "groundplane.h"
#include "game.h"
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>


GroundPlane::GroundPlane() :
    m_tex(GL_INVALID_VALUE)
{
    const int divs = 5;

    // verts
    int numVertRows = divs+1;
    int numVerts = numVertRows * numVertRows;
    m_verts.resize(numVerts);

    const float spacing = 1.0f / numVertRows;
    for (int i = 0; i < numVertRows; ++i)
    {
        for (int j = 0; j < numVertRows; ++j)
        {
            Vector3& vert = m_verts[i*numVertRows + j];
            vert.x = spacing*j - 0.5f;
            vert.y = 0.0f;
            vert.z = spacing*i - 0.5f;
        }
    }

    // texture coords
    m_texCoords.resize(numVerts*2);
    for (int i = 0; i < numVertRows; ++i)
    {
        for (int j = 0; j < numVertRows; ++j)
        {
            float* coord = &m_texCoords[(i*numVertRows + j)*2];
            *coord++ = (float) i / divs;
            *coord++ = (float) j / divs;
        }
    }

    // indices
    int numTris = divs * divs * 2;
    m_faceInds.resize(numTris * 3);

    for (int i = 0; i < divs; ++i)
    {
        for (int j = 0; j < divs; ++j)
        {
            GLubyte* inds = &m_faceInds[(i*divs + j)*6];

            // tri
            *inds++ = (i*numVertRows) + j;
            *inds++ = ((i+1)*numVertRows) + j;
            *inds++ = ((i+1)*numVertRows) + (j+1);

            // tri
            *inds++ = ((i+1)*numVertRows) + (j+1);
            *inds++ = (i*numVertRows) + (j+1);
            *inds++ = (i*numVertRows) + j;

            inds = &m_faceInds[(i*divs + j)*6];
        }
    }
}

GroundPlane::~GroundPlane()
{
    assert(m_tex == GL_INVALID_VALUE); // should have been destroyed!
}

void GroundPlane::initGraphics()
{
    assert(m_tex == GL_INVALID_VALUE);

    // generate texture (random grayscale values)
    int texSize = 128;
    std::vector<uint32_t> texData;
    texData.resize(texSize*texSize);
    for (int i = 0; i < texData.size(); ++i)
    {
        int b = rand() & 0xFF;
        texData[i] = 0xFF000000 | (b << 16) | (b << 8) | b;
    }

    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texData[0]);
}

void GroundPlane::destroyGraphics()
{
    if (m_tex != GL_INVALID_VALUE)
    {
        glDeleteTextures(1, &m_tex);
        m_tex = GL_INVALID_VALUE;
    }
}

void GroundPlane::draw()
{
    glPushMatrix();
    glScalef(200.0f, 1.0f, 200.0f);

    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindTexture(GL_TEXTURE_2D, m_tex);
    glVertexPointer(3, GL_FLOAT, 0, &m_verts[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, &m_texCoords[0]);
    glDrawElements(GL_TRIANGLES, (int) m_faceInds.size(), GL_UNSIGNED_BYTE, &m_faceInds[0]);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

