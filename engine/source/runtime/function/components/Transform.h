#pragma once
#include "EngineMath.h"
#include "Component.h"
#include <memory>
enum Space
{
    //
    // 摘要:
    //     Applies transformation relative to the world coordinate system.
    World,
    //
    // 摘要:
    //     Applies transformation relative to the local coordinate system.
    Self
};

class GameObject;
class Transform : public Component
{
    friend class GameObject;

public:
    Transform();
    Transform(const Vector3& position, const Vector3& rotation, const Vector3& scale);
    ~Transform() = default;
    Transform(Transform const& other) = delete;
    Transform& operator=(Transform& other) noexcept;


    void SetPosition(const Vector3& position);
    void SetEulerAngle(const Vector3& eulerAngle);
    void SetRotation(const Quaternion& rotation);
    void SetScale(const Vector3& scale);

    Vector3 TransformDirection(Vector3 direction) const;
    Vector3 GetPosition() const;
    Vector3 GetLocalPosition() const;
    Quaternion GetRotation() const;
    Quaternion GetLocalRotation() const;
    Vector3 GetEulerAngle() const;
    Vector3 GetLocalEulerAngle() const;
    Vector3 GetScale() const;

    Vector3 forward() const;
    Vector3 right() const;
    Vector3 up() const;
    
    void Rotate(Vector3 eularAngle, Space relativeTo = Space::Self);
    void Rotate(Vector3 axis, float angle, Space relativeTo = Space::Self);
    void RotateAround(Vector3 axis, float angle);
    void RotateAroundLocal(Vector3 axis, float angle);
    void Translate(Vector3 translation, Space relativeTo = Space::Self);
    void LookAt(const Vector3& target, const Vector3& up = Vector3::up);
    Matrix4x4 GetWorldMatrix() const;
protected:
    void Init() override;
private:
    std::shared_ptr<Transform> parent{nullptr};
    Vector3 position;
    // Vector3 rotation;
    Quaternion rotation;
    Vector3 scale;
};