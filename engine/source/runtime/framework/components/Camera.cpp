#include "Camera.h"
#include "core/math/Matrix.h"
#include "core/math/Vector.h"
#include "components/Transform.h"
#include "Shader.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "geometry/Primitive.h"

Camera* Camera::pActivedCamera = nullptr;
std::vector<Camera*> Camera::cameraList{};
Camera::Camera():
    fov(60.0f),
    aspect(1280.0f / 720.0f),
    nearPlane(0.1f),
    farPlane(1000.0f)
{
    if(pActivedCamera == nullptr) pActivedCamera = this;

    cameraList.emplace_back(this);
    updateFrusutmMesh();
}

Camera::~Camera()
{
    
}
void Camera::SetFOV(float fov)
{
    this->fov = fov;
    updateFrusutmMesh();
}

void Camera::SetAspect(float aspect)
{
    this->aspect = aspect;
    updateFrusutmMesh();
}

void Camera::SetNear(float nearPlane)
{
    this->nearPlane = nearPlane;
    updateFrusutmMesh();
}

void Camera::SetFar(float farPlane)
{
    this->farPlane = farPlane;
    updateFrusutmMesh();
}

void Camera::SetOrthSize(float orthSize)
{
    this->orthSize = orthSize;
    updateFrusutmMesh();
}

Mesh * Camera::GetFrustumMesh()
{
    return &frustumMesh;
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
    // auto rotation = pTransform->GetRotation();

    // Matrix4x4 translation = Matrix4x4::Translation(-position);
    // Matrix4x4 Rotation = Matrix4x4::Rotation(rotation).Transpose(); // we want inverse, and for orth matrix : transpose = inverse
    // auto viewMatrix = Rotation * translation;

    return Matrix4x4::LookAtLH(position, position + pTransform->forward(), pTransform->up());
}

Matrix4x4 Camera::GetProjectionMatrix()
{
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


void Camera::updateFrusutmMesh()
{
    if(orth)
    {
        frustumMesh = Primitive::CreateCameraOrthFrustumMesh(orthSize, aspect, nearPlane, farPlane);
        return;
    }else
    {
        frustumMesh = Primitive::CreateCameraFrustumMesh(fov, aspect, nearPlane, farPlane);
    }
}
