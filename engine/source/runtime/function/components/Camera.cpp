#include "Camera.h"
#include "core/math/Matrix.h"
#include "components/Transform.h"

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
    auto position = transform->GetPosition();
    auto rotation = transform->GetRotation();

    Matrix4x4 translation = Matrix4x4::Translation(-position);
    Matrix4x4 Rotation = Matrix4x4::Rotation(rotation).Transpose(); // we want inverse, and for orth matrix : transpose = inverse
    auto viewMatrix = Rotation * translation;
    return viewMatrix;
}

Matrix4x4 Camera::GetProjectionMatrix()
{
    auto projection = Matrix4x4::Perspective(fov, aspect, nearPlane, farPlane);
    return projection;
}