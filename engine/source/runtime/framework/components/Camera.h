#pragma once
#include "Component.h"
class IShaderHelper;
struct Matrix4x4;
struct Vector3;
class Camera : public Component
{
public:
    Camera();
    Camera(const Camera&) = delete;
    ~Camera();

    void SetFOV(float fov);
    void SetAspect(float aspect);
    void SetNear(float _near);
    void SetFar(float _far);
    
    // View Matrix
    Matrix4x4 GetViewMatrix();
    // Projection Matrix
    Matrix4x4 GetProjectionMatrix();
    void LookAt(Vector3 target);
    // static void UpdateCameraBuffer(IShaderHelper& shaderHelper);
    static Camera* pActivedCamera;
protected:
    void Init() override;
    void OnUpdate(float deltaTime) override;
private:
    float fov = 60.0f;
    float aspect = 1.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
};