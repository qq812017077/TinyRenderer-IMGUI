#pragma once
#include "EngineMath.h"

 /**
 * @brief  n * x + d = 0
 * 
 */
struct Plane
{
    Vector3 normal;
    float d;

    float distance(const Vector3& point) const
    {
        return normal.dot(point) + d; // because normal is normalized, so the length of the normal is 1, and the d is the distance from the origin to the plane
    }
    Plane() = default;
    Plane(Vector3& n, float& d)
    {
        auto length = n.length();
        // normalize the plane
        normal /= length;
        this->d /= length;
    }
};


struct Sphere
{
    Vector3 center;
    float radius;
};