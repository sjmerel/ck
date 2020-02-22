#include "ship.h"
#include "gl.h"
#include "ck/sound.h"

Ship::Ship()
{
    m_transform.rotationAxis = Vector3(0.0f, 1.0f, 0.0f);
    m_transform.scale.x = 0.2f;
    m_transform.scale.y = 0.2f;
}

void Ship::setTransform(const Vector3& position, float rotationAngle)
{
    m_transform.translation = position;
    m_transform.rotationAngle = rotationAngle;
}

void Ship::draw()
{
    GLfloat verts[] = 
    {
        -0.5f, 0.5f, 1.0f,
         0.5f, 0.5f, 1.0f,
         0.5f, 0.0f, 1.0f,
        -0.5f, 0.0f, 1.0f,

        0.0f, 0.0f, 0.0f,
    };

    GLfloat colors[] = 
    {
        1.0f, 0.5f, 0.0f, 1.0f,
        1.0f, 0.5f, 0.0f, 1.0f,
        0.5f, 0.2f, 0.0f, 1.0f,
        0.5f, 0.2f, 0.0f, 1.0f,

        1.0f, 1.0f, 1.0f, 1.0f,
    };

    GLubyte inds[] = 
    {
        0, 1, 2,
        2, 3, 0,

        0, 4, 1,
        1, 4, 2,
        2, 4, 3,
        3, 4, 0,
    };


    glPushMatrix();
    glTranslatef(m_transform.translation.x, m_transform.translation.y, m_transform.translation.z);
    glRotatef(m_transform.rotationAngle, m_transform.rotationAxis.x, m_transform.rotationAxis.y, m_transform.rotationAxis.z);
    glScalef(m_transform.scale.x, m_transform.scale.y, m_transform.scale.z);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, verts);
    glColorPointer(4, GL_FLOAT, 0, colors);
    glDrawElements(GL_TRIANGLES, sizeof(inds)/sizeof(GLubyte), GL_UNSIGNED_BYTE, inds);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glPopMatrix();
}



