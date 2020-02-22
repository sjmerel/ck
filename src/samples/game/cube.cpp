#include "cube.h"
#include "gl.h"
#include <stddef.h>
#include <stdlib.h>
#include "ck/sound.h"

Cube::Cube() :
    m_transform(),
    m_angle(0.0f),
    m_sound(NULL)
{
    // looping 3D sound for each cube
    m_sound = CkSound::newBankSound(NULL, "target");
    m_sound->setLoopCount(-1);
    m_sound->setVolume(0.3f);
    m_sound->set3dEnabled(true);

    // randomize the play position so that all the objects don't play in unison!
    int length = m_sound->getLength();
    float startPos = (float) rand() / RAND_MAX; // 0..1
    int startFrame = (int) (length * startPos);
    m_sound->setPlayPosition(startFrame);

    m_sound->play();
}

Cube::~Cube()
{
    m_sound->destroy();
}

void Cube::setPosition(const Vector3& pos)
{
    m_sound->set3dPosition(pos.x, pos.y, pos.z);
    m_transform.translation = pos;
}

const Vector3& Cube::getPosition() const
{
    return m_transform.translation;
}

void Cube::draw(float dt)
{
    GLfloat verts[] = 
    {
        -0.5f, +0.5f, +0.5f,
        +0.5f, +0.5f, +0.5f,
        +0.5f, -0.5f, +0.5f,
        -0.5f, -0.5f, +0.5f,

        -0.5f, +0.5f, -0.5f,
        +0.5f, +0.5f, -0.5f,
        +0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
    };

    GLfloat colors[] = 
    {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,

        0.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
    };

    GLubyte inds[] = 
    {
        0, 2, 1,
        0, 3, 2,

        5, 1, 6,
        1, 2, 6,

        5, 4, 0,
        0, 1, 5,

        0, 4, 7,
        7, 3, 0,

        5, 6, 7,
        7, 4, 5,

        3, 7, 6,
        6, 2, 3
    };


    glPushMatrix();
    glTranslatef(m_transform.translation.x, m_transform.translation.y, m_transform.translation.z);
    glRotatef(m_transform.rotationAngle, m_transform.rotationAxis.x, m_transform.rotationAxis.y, m_transform.rotationAxis.z);
    glScalef(m_transform.scale.x, m_transform.scale.y, m_transform.scale.z);

    // spin the cube
    m_angle += dt * 300.0f;
    if (m_angle > 360.0f)
    {
        m_angle -= 360.0f;
    }
    glRotatef(m_angle, 0.0f, 1.0f, 0.0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, verts);
    glColorPointer(4, GL_FLOAT, 0, colors);
    glDrawElements(GL_TRIANGLES, sizeof(inds)/sizeof(GLubyte), GL_UNSIGNED_BYTE, inds);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glPopMatrix();
}


bool Cube::hitTest(const Vector3& p)
{
    // for simplicity, just doing a sphere test; should really transform point
    // into local coords of cube and test whether it's inside

    Vector3 d;
    Vector3::subtract(p, m_transform.translation, d);
    return Vector3::mag(d) < 0.7f;
}
