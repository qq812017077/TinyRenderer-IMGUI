#include "Camera.h"
#include "core/math/Matrix.h"
#include "core/math/Vector.h"
#include "components/Transform.h"
#include "Shader.h"
#include <d3d11.h>
#include <DirectXMath.h>
Camera* Camera::pActivedCamera = nullptr;
Camera::Camera():
    fov(60.0f),
    aspect(1280.0f / 720.0f),
    nearPlane(0.1f),
    farPlane(1000.0f)
{
    if(pActivedCamera == nullptr) pActivedCamera = this;
}

Camera::~Camera()
{
    
}
void Camera::SetFOV(float fov)
{
    this->fov = fov;
}

void Camera::SetAspect(float aspect)
{
    this->aspect = aspect;
}

void Camera::SetNear(float nearPlane)
{
    this->nearPlane = nearPlane;
}

void Camera::SetFar(float farPlane)
{
    this->farPlane = farPlane;
}

// 
void Camera::OnUpdate(float deltaTime)
{
    
}

void Camera::Init()
{
    
}

Matrix4x4 Camera::GetViewMatrix()
{
    auto position = pTransform->GetPosition();
    auto rotation = pTransform->GetRotation();

    Matrix4x4 translation = Matrix4x4::Translation(-position);
    Matrix4x4 Rotation = Matrix4x4::Rotation(rotation).Transpose(); // we want inverse, and for orth matrix : transpose = inverse
    auto viewMatrix = Rotation * translation;
    return viewMatrix;
}

Matrix4x4 Camera::GetProjectionMatrix()
{
    orthSize = 100.0f;
    auto heightSize = orthSize;
    auto widthSize = orthSize * aspect;
    auto projection = Matrix4x4::Perspective(fov, aspect, nearPlane, farPlane);
    return projection;
}

void Camera::LookAt(Vector3 target)
{
    auto position = pTransform->GetPosition();
    auto rotation = pTransform->GetRotation();
    Vector3 forward = Vector3::Normalize(target - position);
    Vector3 right = Vector3::Normalize(Vector3::Cross(Vector3::up, forward));
    Vector3 up = Vector3::Normalize(Vector3::Cross(forward, right));
    pTransform->SetRotation(Quaternion::LookRotation(forward, up));
}
