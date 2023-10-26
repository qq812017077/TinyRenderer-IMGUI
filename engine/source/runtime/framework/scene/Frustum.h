#pragma once
#include "EngineMath.h"
#include "geometry/geometries.h"
#include "geometry/Bounds.h"

namespace TinyEngine
{
    /**
     * @brief the frustum of the camera in the world space
     * 
     */
    class Frustum
    {
        public:
        enum class Intersection
        {
            OUTSIDE,
            INTERSECT,
            INSIDE
        };
        public:
            Frustum(Matrix4x4 view, Matrix4x4 projection) : Frustum(projection * view) {}
            Frustum(Matrix4x4 VP);

            Intersection ContainSphere(const Vector3& center, const float& radius) const;
            Intersection ContainBounds(Bounds bounds) const;
            bool IntersectAABB(Bounds bounds) const;
            inline Plane& left()  { return planes[0]; }
            inline Plane& right()  { return planes[1]; }
            inline Plane& bottom()  { return planes[2]; }
            inline Plane& top()  { return planes[3]; }
            inline Plane& near() { return planes[4]; }
            inline Plane& far() { return planes[5]; }

            Bounds GetBounds() const { return bounds; }
        private:
        // six plane
        Plane planes[6];
        Bounds bounds;
        // Plane left;
        // Plane right;
        // Plane top;
        // Plane bottom;
        // Plane near;
        // Plane far;
    };
}