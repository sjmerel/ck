#pragma once

#include "vector3.h"

struct Transform
{
public:
    Transform() :
        translation(0.0f, 0.0f, 0.0f),
        rotationAxis(1.0f, 0.0f, 0.0f),
        rotationAngle(0.0f),
        scale(1.0f, 1.0f, 1.0f)
    {}

    Vector3 translation;
    Vector3 rotationAxis;
    float rotationAngle;
    Vector3 scale;
};
