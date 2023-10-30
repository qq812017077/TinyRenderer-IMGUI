#pragma once
#include "EngineMath.h"

struct Bounds
{
public:
    Bounds():
        m_Center(Vector3::zero),
        m_Extents(Vector3::zero)
    {
    }
    Bounds(Vector3 center, Vector3 size)
    {
        m_Center = center;
        m_Extents = size * 0.5f;
    }
    Bounds(Bounds const& other)
    {
        m_Center = other.m_Center;
        m_Extents = other.m_Extents;
    }
    
    Bounds& operator=(const Bounds& other) noexcept
    {
        m_Center = other.m_Center;
        m_Extents = other.m_Extents;
        return *this;
    }

    ~Bounds() = default;

    bool operator==(const Bounds& other) const
    {
        return m_Center == other.m_Center && m_Extents == other.m_Extents;
    }
    bool operator!=(const Bounds& other) const
    {
        return !(*this == other);
    }

    int MaxExtentDim() ;
    
    bool Intersects(const Bounds bounds);
    // bool IntersectRay(Ray ray);
    // bool IntersectRay(Ray ray, float &distance);
    bool IntersectsWithSphere(const Vector3& center, const float& radius);
    
    void Encapsulate(Vector3 point);
    void Encapsulate(Bounds bounds);
    void Expand(float amount);
    void Expand(Vector3 amount);

    bool Contains(Vector3 point);
    float SqrDistance(Vector3 point);
    Vector3 ClosestPoint(Vector3 point);
    Vector3 GetCenter() { return m_Center;  }
    void SetCenter(Vector3 center)  { m_Center = center; }

    Vector3 GetExtents() { return m_Extents; }
    void SetExtents(Vector3 extents) { m_Extents = extents; }

    Vector3 GetSize() { return m_Extents * 2.f; }
    void SetSize(Vector3 size) { m_Extents = size * 0.5f; }

    inline Vector3 GetMin() const { return m_Center - m_Extents; }
    inline void SetMin(Vector3 min) { SetMinMax(min, GetMax()); }

    inline Vector3 GetMax() const { return m_Center + m_Extents; }
    inline void SetMax(Vector3 max) { SetMinMax(GetMin(), max); }
    void SetMinMax(Vector3 min, Vector3 max) { m_Extents = (max - min) * 0.5f; m_Center = min + m_Extents; }

    Bounds Transform(Matrix4x4 transform);
    void GetVertices(Vector3* vertices);
    static Bounds Union(const Bounds& a, const Bounds& b);
    // static bool IntersectRayAABB(Ray ray, Bounds bounds, float &dist);
private:
    Vector3 m_Center;
    Vector3 m_Extents;
};