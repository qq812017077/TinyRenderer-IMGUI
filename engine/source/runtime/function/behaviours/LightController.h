#pragma once
#include "behaviours/Behaviour.h"
#include "ui/GUI.h"
#include "components/Light.h"
#include "input/Input.h"
class LightController : public Behaviour
{
public:
    using Behaviour::Behaviour;

    void Init() override
    {
        pLightGO = GameObject::Find("light");
        pPointLightGO = GameObject::Find("pointLight");
        if(pLightGO) pLight = pLightGO->GetComponent<Light>();
        if(pPointLightGO) pPointLight = pPointLightGO->GetComponent<Light>();
        
        pRedLittleCube = Primitive::CreateCube("red littleCube");
        pRedLittleCube->transform.SetScale({ 0.1f, 0.1f, 0.1f });
        pRedLittleCube->GetComponent<Renderer>()->GetMaterial()->SetColor("color", Color::Red());

        if(pLight)
        {
            pLight->pTransform->SetEulerAngle({ 90.0f, 0.0f, 0.0f });
            eulerAngle = pLight->pTransform->GetEulerAngle();
        }
        
        if(pPointLight)
        {
            pWhiteLittleCube = Primitive::CreateCube("white littleCube");
            pWhiteLittleCube->GetComponent<Renderer>()->GetMaterial()->SetColor("color", Color::White());
            pWhiteLittleCube->transform.SetPosition({ 0.0f, 1.5f, 0.0f });
            pWhiteLittleCube->transform.SetScale({ 0.1f, 0.1f, 0.1f });
            
            pointLightPos = { 0.3f, 1.5f, 0.0f };
            pPointLight->pTransform->SetPosition(pointLightPos);
            pPointLight->SetIntensity(1.0f);
        }
    }

    void OnUpdate(float deltaTime) override
    {
        if(pLight)
        {
            pLight->pTransform->SetEulerAngle(eulerAngle);
            if(pRedLittleCube != nullptr)
                pRedLittleCube->transform.SetPosition(pLight->pTransform->forward() * 1.0f);
            pLight->SetIntensity(intensity);
        }

        if(pPointLight)
        {
            pPointLight->pTransform->SetPosition(pointLightPos);
            pWhiteLittleCube->transform.SetPosition(pointLightPos);
        }
        
    }

    void OnGUI() override
    {
        if(pLight)
        {
            GUI::Begin("Directional Light");
            GUI::SliderFloat("Euler Angle x", &eulerAngle.x, -180.0f, 180.0f);
            GUI::SliderFloat("Euler Angle y", &eulerAngle.y, -180.0f, 180.0f);
            GUI::SliderFloat("Euler Angle z", &eulerAngle.z, -180.0f, 180.0f);
            GUI::SliderFloat("Intensity", &intensity, 0.0f, 1.0f);
            auto & qua = pLight->pTransform->GetRotation();
            GUI::Text("Light quaternion: (%.2f, %.2f, %.2f, %.2f)", qua.x, qua.y, qua.z, qua.w);
            //show Light direction
            auto & forward = pLight->pTransform->forward();
            GUI::Text("Light Direction: (%.2f, %.2f, %.2f)", forward.x, forward.y, forward.z);
            GUI::End();
        }
        
        if(pPointLight)
        {
            GUI::Begin("Point Light");
            // update pointLightPos
            GUI::DragFloat3("Point Light Position", reinterpret_cast<float*>(&pointLightPos), 0.1f);
            GUI::End();
        }
    }

private:
    GameObject * pLightGO;
    GameObject * pPointLightGO;
    Light * pLight;
    Light * pPointLight;
    Vector3 eulerAngle;
    float intensity = 1.0f;
    GameObject * pRedLittleCube;
    Vector3 pointLightPos;
    GameObject * pWhiteLittleCube;
};