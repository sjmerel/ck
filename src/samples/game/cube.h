#pragma once
#include "transform.h"
#include "vector3.h"

class CkSound;

class Cube 
{
public:
    Cube();
    ~Cube();

    void setPosition(const Vector3&);
    const Vector3& getPosition() const;

    void draw(float dt);
    bool hitTest(const Vector3&);

private:
    float m_angle;
    Transform m_transform;
    CkSound* m_sound;
};


