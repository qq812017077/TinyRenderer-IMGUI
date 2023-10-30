#pragma once
#include "EngineMath.h"

 /**
 * @brief  n * x + d = 0
 * 
 */
struct Plane
{
    Vector3 normal;
    float d{0.0f};

    float distance(const Vector3& point) const
    {
        return normal.dot(point) + d; // because normal is normalized, so the length of the normal is 1, and the d is the distance from the origin to the plane
    }
    Plane() = default;
    Plane(Vector3& _n, float& _d) : normal(_n), d(_d)
    {
        auto length = normal.length();
        // normalize the plane
        normal /= length;
        d /= length;
    }
};


struct Sphere
{
    Vector3 center;
    float radius;
};