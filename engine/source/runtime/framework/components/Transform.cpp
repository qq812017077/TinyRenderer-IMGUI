#include "Transform.h"
#include <cmath>
#include "Quaternion.h"
#include "object/GameObject.h"
Transform::Transform():
    position(0.0f,0.0f,0.0f),
    rotation(0.0f,0.0f,0.0f,1.0f),
    scale({1.0f,1.0f,1.0f})
{

}

Transform::Transform(const Vector3& position, const Vector3& rotation, const Vector3& scale):
    position(position),
    rotation(Quaternion::Euler(rotation)),
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
Transform& Transform::operator=(const Transform& other) noexcept
{
    position = other.position;
    rotation = other.rotation;
    scale = other.scale;
    return *this;
}

void Transform::SetPosition(const Vector3& position)
{
    if(parent)
    {
        // get local position
        this->position = parent->GetWorldMatrix().Inverse() * Vector4(position, 1.0f);
    }
    else
    {
        this->position = position;
    }
}
void Transform::SetLocalPosition(const Vector3& position)
{
    this->position = position;
}

void Transform::SetEulerAngle(const Vector3& eulerAngle)
{
    rotation = Quaternion::Euler(eulerAngle);
    if (parent)
    {
        // get local rotation
        rotation = Quaternion::Inverse(parent->GetRotation()) * rotation;
    }
}
void Transform::SetLocalEulerAngle(const Vector3& eulerAngle)
{
    this->rotation = Quaternion::Euler(eulerAngle);
}

void Transform::SetRotation(const Quaternion& rotation)
{
    if (parent)
    {
        // get local rotation
        auto inv = Quaternion::Inverse(parent->GetRotation());
        this->rotation = inv * rotation;
    }
    else
    {
        this->rotation = rotation;
    }
}
void Transform::SetLocalRotation(const Quaternion& rotation)
{
    this->rotation = rotation;
}
void Transform::SetScale(const Vector3& scale)
{
    this->scale = scale;
}



Vector3 Transform::GetPosition() const
{
    if(parent)
    {
        // get world position
        return parent->GetWorldMatrix() * Vector4(position, 1.0f);
    }
    return position;
}

Vector3 Transform::GetLocalPosition() const
{
    return position;
}

Vector3 Transform::GetEulerAngle() const
{
    if (parent)
    {
        // get world rotation
        return (parent->GetRotation() * rotation).EulerAngles();
    }
    return rotation.EulerAngles();
}

Vector3 Transform::GetLocalEulerAngle() const
{
    return rotation.EulerAngles();
}

Quaternion Transform::GetRotation() const
{
    if (parent)
    {
        // get world rotation
        return parent->GetRotation() * rotation;
    }
    return rotation;
}

Quaternion Transform::GetLocalRotation() const
{
    return rotation;
}

Vector3 Transform::GetScale() const
{
    return scale;
}
Vector3 Transform::GetLossyScale() const
{
    if (parent)
    {
        // get world scale
        return parent->GetLossyScale() * scale;
    }
    return scale;
}

Vector3 Transform::TransformDirection(Vector3 direction) const
{
    return Matrix3x3::Rotation(rotation) * direction;
}


Vector3 Transform::forward() const
{
    return Matrix3x3::Rotation(rotation) * Vector3::forward;
}

Vector3 Transform::right() const
{
    return Matrix3x3::Rotation(rotation) * Vector3::right;
}

Vector3 Transform::up() const
{
    return Matrix3x3::Rotation(rotation) * Vector3::up;
}


void Transform::Rotate(Vector3 eularAngle, Space relativeTo)
{
    Quaternion quaternion = Quaternion::Euler(eularAngle.x, eularAngle.y, eularAngle.z);
    if (relativeTo == Space::Self)
    {
        rotation *= quaternion;
    }
    else
    {
        rotation *= Quaternion::Inverse(rotation) * quaternion * rotation;
    }
}

void Transform::Rotate(Vector3 axis, float angle, Space relativeTo)
{
    
    if (relativeTo == Space::Self)
    {
        RotateAroundLocal(TransformDirection(axis), angle * kDeg2Rad);
    }
    else
    {
        RotateAround(axis, angle * kDeg2Rad);
    }
}

void Transform::RotateAround(Vector3 axis, float angle)
{
    Quaternion quaternion = Quaternion::GetQuaternionFromAxisAngle(axis, angle);
    position = quaternion * position;
    rotation = quaternion * rotation;
}

void Transform::RotateAroundLocal(Vector3 axis, float angle)
{
    Quaternion quaternion = Quaternion::GetQuaternionFromAxisAngle(axis, angle);
    position = quaternion * position;
    rotation = rotation * quaternion;
}

void Transform::Translate(Vector3 translation, Space relativeTo)
{
    if (relativeTo == Space::Self)
    {
        auto rotMat = Matrix3x3::Rotation(rotation);
        position += rotMat * translation;
    }
    else
    {
        position += translation;
    }
}

void Transform::LookAt(const Vector3& target, const Vector3& up)
{
    Vector3 forward = target - position;
    Vector3 right = Vector3::Cross(up, forward);

    if (Vector3::Approximately(right, Vector3::zero))
    {
        if (forward.z > 0)
        {
            rotation = Quaternion::Euler(0, 0, 0);
        }
        else
        {
            rotation = Quaternion::Euler(180, 0, 0);
        }
    }
    else
    {
        right.normalize();
        forward.normalize();
        auto _up = Vector3::Cross(forward, right);
        Matrix3x3 matrix = Matrix3x3::Identity();
        matrix[0][0] = right.x;
        matrix[0][1] = right.y;
        matrix[0][2] = right.z;
        matrix[1][0] = _up.x;
        matrix[1][1] = _up.y;
        matrix[1][2] = _up.z;
        matrix[2][0] = forward.x;
        matrix[2][1] = forward.y;
        matrix[2][2] = forward.z;
        rotation = Quaternion::FromRotationMatrix(matrix);
    }
}

Matrix4x4 Transform::GetWorldMatrix() const
{
    
    if(parent) return parent->GetWorldMatrix() * Matrix4x4::TRS(position, rotation, scale);
    return Matrix4x4::TRS(position, rotation, scale);
}


void Transform::SetParent(Transform& parent, bool worldPositionStays)
{
    if (worldPositionStays)
    {
        auto worldPos = GetPosition();
        auto worldRot = GetRotation();
        auto worldScale = GetScale();
        this->parent = &parent;
        SetPosition(worldPos);
        SetRotation(worldRot);
        SetScale(worldScale);
    }
    else
        this->parent = &parent;

    parent.children.push_back(this);
}

Transform* Transform::GetParent() const
{
    return parent;
}

Transform* Transform::FindChild(const char * name) const
{
    for(auto & pChild : children)
    {
        auto pGameObject = pChild->pGameObject;
        auto name = pGameObject->GetName();
        if(name == name) return pChild;
    }
    return nullptr;
}

Transform* Transform::GetChild(int index) const
{
    if(index < children.size()) return children[index];
    return nullptr;
}

bool Transform::IsChildOf(Transform * parent) const
{
    return false;
}

int Transform::GetChildCount() const
{
    return children.size();
}
void Transform::DetachChildren()
{
    for(auto child : children)
    {
        child->parent = nullptr;
    }
    
    children.clear();
}

void Transform::Init()
{
    // do nothing
}