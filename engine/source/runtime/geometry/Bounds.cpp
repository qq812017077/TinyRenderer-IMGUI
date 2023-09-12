#include "Bounds.h"

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