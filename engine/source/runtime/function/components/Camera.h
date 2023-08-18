#pragma once
#include "core/GameObject.h"

class Camera : public GameObject
{
public:
    Camera() ;
    Camera(std::string name) ;
    Camera(const Camera&) = delete;
    ~Camera() = default;


    void SetFOV(float fov);
    void SetAspect(float aspect);
    void SetNear(float _near);
    void SetFar(float _far);

    void Init() override;

    // View Matrix
    Matrix4x4 GetViewMatrix();

    // Projection Matrix
    Matrix4x4 GetProjectionMatrix();

    void OnUpdate(float deltaTime) override;

    static Camera* pActivedCamera;
private:
    float fov = 60.0f;
    float aspect = 1.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
};