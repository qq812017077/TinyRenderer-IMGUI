#pragma once
#include "EngineMath.h"

class Transform
{
public:
    Transform();
    Transform(const Vector3& position, const Vector3& rotation, const Vector3& scale);
    ~Transform() = default;
    Transform(Transform const& other) = delete;
    Transform& operator=(Transform& other) noexcept;

    void SetPosition(const Vector3& position);
    void SetRotation(const Vector3& rotation);
    void SetScale(const Vector3& scale);

    Matrix4x4 GetWorldMatrix() const;
    Vector3 GetPosition() const;
    Vector3 GetRotation() const;
    Vector3 GetScale() const;

private:
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
};