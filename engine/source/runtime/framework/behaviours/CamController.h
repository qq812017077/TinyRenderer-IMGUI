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
        pCamera = pGameObject->GetComponent<Camera>();
        pCamera->SetAspect(1280.0f / 720.0f);
        pTransform->SetPosition({ 0.0f, 3.0f, -6.0f });
        pTransform->SetEulerAngle({ 30.0f, 0.0f, 0.0f });
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

        // bool isMouseLeftDown = Input::InputSystem::GetMouseButton(Mouse::MouseButton::Left);
        bool isMouseRightDown = Input::InputSystem::GetMouseButton(Mouse::MouseButton::Right);
        bool isMouseMiddleDown = Input::InputSystem::GetMouseButton(Mouse::MouseButton::Middle);
        float scroll = Input::InputSystem::GetMouseScrollWheel();
        // std::cout << "scroll: " << scroll << std::endl;
        // std::cout << "isMouseRightDown: " << isMouseRightDown << std::endl;
        // std::cout << "isMouseMiddleDown: " << isMouseMiddleDown << std::endl;
        float dx = Input::InputSystem::GetMouseAxisX();
        float dy = Input::InputSystem::GetMouseAxisY();
        if(isMouseMiddleDown)
        {
            pCamera->pTransform->Translate(-Vector3::up * dy * moveSpeed * deltaTime);
            pCamera->pTransform->Translate(-Vector3::right * dx * moveSpeed * deltaTime);
        }else if(isMouseRightDown)
        {
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
        }else if (scroll > kEpsilon || scroll < -kEpsilon)
        {
            pCamera->pTransform->Translate(Vector3::forward * scroll * moveSpeed * deltaTime);
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