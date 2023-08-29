#pragma once
#include "behaviours/Behaviour.h"
#include "ui/GUI.h"
#include "components/Camera.h"
#include "input/Input.h"
class CamController : public Behaviour
{
public:
    using Behaviour::Behaviour;

    void Init() override
    {
        pCamera = owner->GetComponent<Camera>();
    }

    void OnUpdate(float deltaTime) override
    {
        //Get keyboard and mouse input
        //Update camera transform
        if(Input::InputSystem::GetKey(Input::KeyCode::S))
        {
            pCamera->pTransform->Translate(-Vector3::forward * deltaTime * moveSpeed);
        }

        if (Input::InputSystem::GetKey(Input::KeyCode::W))
        {
            pCamera->pTransform->Translate(Vector3::forward * deltaTime * moveSpeed);
        }

        if (Input::InputSystem::GetKey(Input::KeyCode::D))
        {
            pCamera->pTransform->Translate(Vector3::right * deltaTime * moveSpeed);
        }

        if (Input::InputSystem::GetKey(Input::KeyCode::A))
        {
            pCamera->pTransform->Translate(-Vector3::right * deltaTime * moveSpeed);
        }

        if(Input::InputSystem::GetMouseButton(Mouse::MouseButton::Right))
        {
            float dx = Input::InputSystem::GetMouseAxisX();
            float dy = Input::InputSystem::GetMouseAxisY();
            auto euler = pCamera->pTransform->GetEulerAngle();
            euler.x -= dy * rotateSpeed;
            euler.y += dx * rotateSpeed;
            euler.z = 0.0f;
            pCamera->pTransform->SetEulerAngle(euler);
            // pCamera->transform->Rotate(Vector3(-1, 0, 0) * dy * rotateSpeed);
            // pCamera->transform->Rotate(Vector3(0, 1, 0) * dx * rotateSpeed);
            auto & rotation = pCamera->pTransform->GetRotation();
            // std::cout << "x:" << rotation.x << " y: " << rotation.y << " z: " << rotation.z << " w: " << rotation.w << std::endl;
            // std::cout << "dx: " << dx << " dy: " << dy << std::endl;
        }
    }

    void OnGUI() override
    {
        GUI::Begin("Camera");
        // move speed
        GUI::SliderFloat("move speed", &moveSpeed, 0.0f, 100.0f);
        GUI::SliderFloat("rotate speed", &rotateSpeed, 0.0f, 10.0f);
        auto position = pCamera->pTransform->GetPosition();
        auto eulerAngle = pCamera->pTransform->GetEulerAngle();
        auto quat = pCamera->pTransform->GetRotation();
        GUI::Text("Camera position: %f, %f, %f", position.x, position.y, position.z);
        GUI::Text("Camera rotation: %f, %f, %f", eulerAngle.x, eulerAngle.y, eulerAngle.z);
        GUI::Text("Camera Quat: %f, %f, %f, %f", quat.x, quat.y, quat.z, quat.w);
        GUI::End();
    }

private:
    Camera * pCamera;
    float moveSpeed = 10.0f;
    float rotateSpeed = 4.0f;
};