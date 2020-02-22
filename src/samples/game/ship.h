#pragma once
#include "transform.h"

class Ship
{
public:
    Ship();

    void setTransform(const Vector3& position, float rotationAngle);

    void draw();

private:
    Transform m_transform;

};



