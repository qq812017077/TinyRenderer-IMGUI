#include "Bounds.h"


int Bounds::MaxExtentDim()
{
    if(m_Extents.x > m_Extents.y && m_Extents.x > m_Extents.z)
        return 0;
    else if(m_Extents.y > m_Extents.z)
        return 1;
    else
        return 2;
}

bool Bounds::Intersects(const Bounds bounds)
{
    auto min = GetMin();
    auto max = GetMax();
    auto boundsMin = bounds.GetMin();
    auto boundsMax = bounds.GetMax();
    return  min.x <= boundsMax.x && max.x >= boundsMin.x && 
            min.y <= boundsMax.y && max.y >= boundsMin.y && 
            min.z <= boundsMax.z && max.z >= boundsMin.z;
}


bool Bounds::IntersectsWithSphere(const Vector3& center, const float& radius)
{
    auto min_bound = GetMin();
    auto max_bound = GetMax();
    for (size_t i = 0; i < 3; ++i)
    {
        if (center[i] < min_bound[i])
        {
            if ((min_bound[i] - center[i]) > radius)
            {
                return false;
            }
        }
        else if (center[i] > max_bound[i])
        {
            if ((center[i] - max_bound[i]) > radius)
            {
                return false;
            }
        }
    }
    return true;
}

void Bounds::Encapsulate(Vector3 point)
{
    SetMinMax(Vector3::Min(GetMin(), point), Vector3::Max(GetMax(), point));
}

void Bounds::Encapsulate(Bounds bounds)
{
    Encapsulate(bounds.m_Center - bounds.m_Extents);
    Encapsulate(bounds.m_Center + bounds.m_Extents);
}

void Bounds::Expand(float amount)
{
    amount *= 0.5f;
    m_Extents += Vector3(amount, amount, amount);
}

void Bounds::Expand(Vector3 amount)
{
    m_Extents += amount * 0.5f;
}

bool Bounds::Contains(Vector3 point)
{
    auto min = GetMin();
    auto max = GetMax();
    return  min.x <= point.x && max.x >= point.x &&
            min.y <= point.y && max.y >= point.y &&
            min.z <= point.z && max.z >= point.z;
}

float Bounds::SqrDistance(Vector3 point)
{
    auto min = GetMin();
    auto max = GetMax();
    float sqrDistance = 0.0f;
    float delta = 0.0f;
    if(point.x < min.x)
    {
        delta = point.x - min.x;
        sqrDistance += delta * delta;
    }
    else if(point.x > max.x)
    {
        delta = point.x - max.x;
        sqrDistance += delta * delta;
    }
    if(point.y < min.y)
    {
        delta = point.y - min.y;
        sqrDistance += delta * delta;
    }
    else if(point.y > max.y)
    {
        delta = point.y - max.y;
        sqrDistance += delta * delta;
    }
    if(point.z < min.z)
    {
        delta = point.z - min.z;
        sqrDistance += delta * delta;
    }
    else if(point.z > max.z)
    {
        delta = point.z - max.z;
        sqrDistance += delta * delta;
    }
    return sqrDistance;
}
Vector3 Bounds::ClosestPoint(Vector3 point)
{
    auto min = GetMin();
    auto max = GetMax();
    Vector3 closestPoint = point;
    if(point.x < min.x)
        closestPoint.x = min.x;
    else if(point.x > max.x)
        closestPoint.x = max.x;
    if(point.y < min.y)
        closestPoint.y = min.y;
    else if(point.y > max.y)
        closestPoint.y = max.y;
    if(point.z < min.z)
        closestPoint.z = min.z;
    else if(point.z > max.z)
        closestPoint.z = max.z;
    return closestPoint;
}


Bounds Bounds::Transform(Matrix4x4 transform)
{
    const Vector3 g_BoxOffset[8] = {Vector3(-1.0f, -1.0f, 1.0f),
                                        Vector3(1.0f, -1.0f, 1.0f),
                                        Vector3(1.0f, 1.0f, 1.0f),
                                        Vector3(-1.0f, 1.0f, 1.0f),
                                        Vector3(-1.0f, -1.0f, -1.0f),
                                        Vector3(1.0f, -1.0f, -1.0f),
                                        Vector3(1.0f, 1.0f, -1.0f),
                                        Vector3(-1.0f, 1.0f, -1.0f)};

    size_t const CORNER_COUNT = 8;

    Vector3 min;
    Vector3 max;

    // Compute and transform the corners and find new min/max bounds.
    for (size_t i = 0; i < CORNER_COUNT; ++i)
    {
        Vector3 corner_before = m_Extents * g_BoxOffset[i] + m_Center;
        Vector4 corner_with_w = transform * Vector4(corner_before.x, corner_before.y, corner_before.z, 1.0f);
        Vector3 corner        = Vector3(corner_with_w.x / corner_with_w.w,
                                        corner_with_w.y / corner_with_w.w,
                                        corner_with_w.z / corner_with_w.w);

        if (0 == i)
        {
            min = corner;
            max = corner;
        }
        else
        {
            min = Vector3::Min(min, corner);
            max = Vector3::Max(max, corner);
        }
    }

    Bounds out;
    out.SetMinMax(min, max);
    return out;
}

void Bounds::GetVertices(Vector3* vertices)
{
    auto min = GetMin();
    auto max = GetMax();
    vertices[0] = Vector3(min.x, min.y, min.z);
    vertices[1] = Vector3(min.x, min.y, max.z);
    vertices[2] = Vector3(min.x, max.y, min.z);
    vertices[3] = Vector3(min.x, max.y, max.z);
    vertices[4] = Vector3(max.x, min.y, min.z);
    vertices[5] = Vector3(max.x, min.y, max.z);
    vertices[6] = Vector3(max.x, max.y, min.z);
    vertices[7] = Vector3(max.x, max.y, max.z);

}

Bounds Bounds::Union(const Bounds& a, const Bounds& b)
{
    Bounds bounds = a;
    bounds.Encapsulate(b);
    return bounds;
}