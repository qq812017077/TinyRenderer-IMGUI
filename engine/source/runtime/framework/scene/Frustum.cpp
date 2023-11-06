#include "Frustum.h"
#include "geometry/Bounds.h"

namespace TinyEngine
{
    using Intersection = Frustum::Intersection;

    /**
     * @brief 
     * // [Fast Extraction of Viewing Frustum Planes from the WorldView - Projection
     *     Matrix](http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf)
     * 
     * @param VP 
     */
    Frustum::Frustum(Matrix4x4 VP)
    {
        // left
        Vector3 normal;
        float d;

        normal.x = VP[3][0] + VP[0][0];
        normal.y = VP[3][1] + VP[0][1];
        normal.z = VP[3][2] + VP[0][2];
        d = VP[3][3] + VP[0][3];
        left() = Plane(normal, d);

        // right
        normal.x = VP[3][0] - VP[0][0];
        normal.y = VP[3][1] - VP[0][1];
        normal.z = VP[3][2] - VP[0][2];
        d = VP[3][3] - VP[0][3];
        right() = Plane(normal, d);

        // bottom
        normal.x = VP[3][0] + VP[1][0];
        normal.y = VP[3][1] + VP[1][1];
        normal.z = VP[3][2] + VP[1][2];
        d = VP[3][3] + VP[1][3];
        bottom() = Plane(normal, d);
        // top
        normal.x = VP[3][0] - VP[1][0];
        normal.y = VP[3][1] - VP[1][1];
        normal.z = VP[3][2] - VP[1][2];
        d = VP[3][3] - VP[1][3];
        top() = Plane(normal, d);

        // near
        normal.x = VP[2][0];
        normal.y = VP[2][1];
        normal.z = VP[2][2];
        d =  VP[2][3];
        Near() = Plane(normal, d);

        // far
        normal.x = VP[3][0] - VP[2][0];
        normal.y = VP[3][1] - VP[2][1];
        normal.z = VP[3][2] - VP[2][2];
        d = VP[3][3] - VP[2][3];
        Far() = Plane(normal, d);

        // get eight corners of the frustum from view projection matrix
        Vector3 corners_ndc[8] = {
            Vector3(-1, -1, 0),
            Vector3(-1,  1, 0),
            Vector3( 1,  1, 0),
            Vector3( 1, -1, 0),
            Vector3(-1, -1,  1),
            Vector3(-1,  1,  1),
            Vector3( 1,  1,  1),
            Vector3( 1, -1,  1)
        };

        // transform the corners from ndc space to world space
        Matrix4x4 invVP = VP.Inverse();
        Vector3 min{Vector3::max}, max{Vector3::min};
        for(int i = 0; i < 8; ++i)
        {
            Vector4 point = invVP * Vector4(corners_ndc[i], 1.0f);
            Vector3 worldPos = Vector3(point.x, point.y, point.z) / point.w; // divide by w, because w must be one.
            m_corners[i] = worldPos;
            min = Vector3::Min(min, worldPos);
            max = Vector3::Max(max, worldPos);
        }

        bounds.SetMinMax(min, max);

    }
    
    
    Frustum::Frustum(const Frustum& frustum)
    {
        for(int i = 0 ; i < 6; i++)
            planes[i] = frustum.planes[i];
        bounds = frustum.bounds;
        for(int i = 0 ; i < 8; i++)
            m_corners[i] = frustum.m_corners[i];
    }

    
    Frustum& Frustum::operator=(const Frustum& other)
    {
        Frustum frustum(other);
        return std::move(frustum);
    }

    Intersection Frustum::ContainSphere(const Vector3& center, const float& radius) const
    {
        float dist;
        for(int i = 0; i < 6; ++i) {
            dist = planes[i].distance(center);
            if(dist > radius) return Intersection::OUTSIDE;
            if(std::fabsf(dist) < radius) return Intersection::INTERSECT;
        }
        return Intersection::INSIDE;
    }

    static Vector3 corners[8];
    Intersection Frustum::ContainBounds(Bounds bounds) const
    {
        bounds.GetCorners(corners);
        float dist;
        int inCount{};
        for(int i = 0; i < 6; ++i) {
            int out{0};
            bool in{true};
            for(int i = 0; i < 8; ++i)
            {
                dist = planes[i].distance(corners[i]);
                if(dist > 0) 
                {
                    out++;
                    in = false;
                }
            }
            if(out == 8) return Intersection::OUTSIDE;
            if(in) inCount++;
        }
        if(inCount == 6) return Intersection::INSIDE;
        return Intersection::INTERSECT;
    }

    
    bool Frustum::IntersectAABB(Bounds bounds) const
    {
        bounds.GetCorners(corners);
        float dist;
        for(int i = 0; i < 6; ++i) {
            int out{0};
            for(int j = 0; j < 8; ++j)
            {
                dist = planes[i].distance(corners[j]);
                if(dist < 0) out++;
            }
            if(out == 8) return false;
            
        }
        return true;
    }
}

