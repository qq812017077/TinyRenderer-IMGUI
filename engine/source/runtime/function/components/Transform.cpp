#include "Transform.h"

Transform::Transform():
    position(0.0f,0.0f,0.0f),
    rotation(0.0f,0.0f,0.0f),
    scale({1.0f,1.0f,1.0f})
{

}

Transform::Transform(const Vector3& position, const Vector3& rotation, const Vector3& scale):
    position(position),
    rotation(rotation),
    scale(scale)
{
}   

Transform& Transform::operator=(Transform& other) noexcept
{
    position = other.position;
    rotation = other.rotation;
    scale = other.scale;
    return *this;
}

void Transform::SetPosition(const Vector3& position)
{
    this->position = position;
}

void Transform::SetRotation(const Vector3& rotation)
{
    this->rotation = rotation;
}

void Transform::SetScale(const Vector3& scale)
{
    this->scale = scale;
}

Vector3 Transform::GetPosition() const
{
    return position;
}

Vector3 Transform::GetRotation() const
{
    return rotation;
}

Vector3 Transform::GetScale() const
{
    return scale;
}

void Transform::Rotate(const Vector3& axis, float angle)
{
    rotation += axis * angle;
}

Vector3 Transform::forward() const
{
    return Matrix3x3::Rotation(rotation) * Vector3::Forward();
}

Vector3 Transform::right() const
{
    return Matrix3x3::Rotation(rotation) * Vector3::Right();
}

Vector3 Transform::up() const
{
    return Matrix3x3::Rotation(rotation) * Vector3::Up();
}

Matrix4x4 Transform::GetWorldMatrix() const
{
    Matrix4x4 translation = Matrix4x4::Translation(position);
    Matrix4x4 rotation = Matrix4x4::Rotation(this->rotation);
    Matrix4x4 scale = Matrix4x4::Scale(this->scale);
    return  translation * rotation * scale;
}