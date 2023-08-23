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
        pCamera = owner->AddComponent<Camera>();
    }

    void OnUpdate(float deltaTime) override
    {
        //Get keyboard and mouse input
        //Update camera transform
        if(Input::InputSystem::GetKey(Input::KeyCode::S))
        {
            pCamera->transform->Translate(Vector3(0, 0, -1) * deltaTime * moveSpeed);
        }

        if (Input::InputSystem::GetKey(Input::KeyCode::W))
        {
            pCamera->transform->Translate(Vector3(0, 0, 1) * deltaTime * moveSpeed);
        }

        if (Input::InputSystem::GetKey(Input::KeyCode::D))
        {
            pCamera->transform->Translate(Vector3(1, 0, 0) * deltaTime * moveSpeed);
        }

        if (Input::InputSystem::GetKey(Input::KeyCode::A))
        {
            pCamera->transform->Translate(Vector3(-1, 0, 0) * deltaTime * moveSpeed);
        }

        if(Input::InputSystem::GetMouseButton(Mouse::MouseButton::Right))
        {
            float dx = Input::InputSystem::GetMouseAxisX();
            float dy = Input::InputSystem::GetMouseAxisY();
            pCamera->transform->Rotate(Vector3(-1, 0, 0) * dy * rotateSpeed);
            pCamera->transform->Rotate(Vector3(0, 1, 0) * dx * rotateSpeed);
            // std::cout << "dx: " << dx << " dy: " << dy << std::endl;
        }
    }

    void OnGUI() override
    {
        GUI::Begin("Camera");
        // move speed
        GUI::SliderFloat("move speed", &moveSpeed, 0.0f, 100.0f);
        GUI::SliderFloat("rotate speed", &rotateSpeed, 0.0f, 100.0f);
        auto position = pCamera->transform->GetPosition();
        auto eulerAngle = pCamera->transform->GetEulerAngle();
        auto quat = pCamera->transform->GetRotation();
        GUI::Text("Camera position: %f, %f, %f", position.x, position.y, position.z);
        GUI::Text("Camera rotation: %f, %f, %f", eulerAngle.x, eulerAngle.y, eulerAngle.z);
        GUI::Text("Camera Quat: %f, %f, %f, %f", quat.x, quat.y, quat.z, quat.w);
        GUI::End();
    }

private:
    Camera * pCamera;
    float moveSpeed = 10.0f;
    float rotateSpeed = 20.0f;
};