#include "Camera.h"
#include <DirectXMath.h>

Camera::Camera():
    Camera("Camera")
{
}

Camera::Camera(std::string name):
    GameObject(name),
    fov(60.0f),
    aspect(1280.0f / 720.0f),
    nearPlane(0.1f),
    farPlane(1000.0f)
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
    // static float curTime = 0.0f;
    // curTime += deltaTime;
    // // // move camera by sin and cos
    // auto position = transform.GetPosition();
    // // position.x = sin(curTime) * 1.0f;
    // position.z = cos(curTime) * 5.0f;
    // transform.SetPosition(position);
}

void Camera::Init()
{
    
}

Matrix4x4 Camera::GetViewMatrix()
{
    auto position = transform.GetPosition();
    auto rotation = transform.GetRotation();

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