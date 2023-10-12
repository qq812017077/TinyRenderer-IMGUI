#pragma once
#include "behaviours/Behaviour.h"
#include "ui/GUI.h"
#include "components/Light.h"
#include "input/Input.h"
#include "Texture.h"
#include "object/GameObject.h"
class LightController : public Behaviour
{
public:
    using Behaviour::Behaviour;

    void Init() override
    {
        auto pMat = Material::Create("Unlit", "Unlit-Material");
        pLightGO = Light::CreateDirectionalLight("dirLight");
        pPointLightGO = Light::CreatePointLight("pointLight");
        if(pLightGO) pLight = pLightGO->GetComponent<Light>();
        if(pPointLightGO) pPointLight = pPointLightGO->GetComponent<Light>();
        
        if(pLight)
        {
            pLight->pTransform->SetEulerAngle({ 90.0f, 0.0f, 0.0f });
            pLight->SetIntensity(0.2f);
        }
        
        if(pPointLight)
        {
            pWhiteLittleCube = Primitive::CreateCube("white littleCube");
            pWhiteLittleCube->GetComponent<Renderer>()->SetMaterial(pMat);
            pWhiteLittleCube->GetComponent<Renderer>()->GetMaterial()->SetColor("color", Color::White());
            pWhiteLittleCube->transform().SetPosition({ 0.0f, 0.f, 0.0f });
            pWhiteLittleCube->transform().SetScale({ 1.f, 1.f, 1.f });
            pWhiteLittleCube->transform().SetParent(pPointLightGO->transform());
            pWhiteLittleCube->transform().SetLocalPosition({ 0.0f, 0.0f, 0.0f });
            pointLightPos = { 0.3f, 1.5f, 0.0f };
            pPointLight->pTransform->SetPosition({ 0.0f, 120.f, 0.0f });
            pPointLight->SetIntensity(0.8f);
            pPointLight->SetRange(500.0f);
        }
    }

    void OnUpdate(float deltaTime) override
    {
        
    }

    void OnGUI() override
    {
        GUI::Begin("Lights");
        if(pLight)
        {
            GUI::Separator();
            bool active = pLight->GetGameObject()->IsActived();
            GUI::Checkbox("Active", &active);
            pLight->GetGameObject()->SetActive(active);
            auto eulerAngle = pLight->pTransform->GetEulerAngle();
            GUI::SliderFloat("Euler Angle x", &eulerAngle.x, -180.0f, 180.0f);
            GUI::SliderFloat("Euler Angle y", &eulerAngle.y, -180.0f, 180.0f);
            GUI::SliderFloat("Euler Angle z", &eulerAngle.z, -180.0f, 180.0f);
            auto intensity = pLight->GetIntensity();
            GUI::SliderFloat("Dir Intensity", &intensity, 0.0f, 1.0f);
            pLight->SetIntensity(intensity);
            pLight->pTransform->SetEulerAngle(eulerAngle);
        }
        if(pPointLight)
        {
            GUI::Separator();
            bool active = pPointLight->GetGameObject()->IsActived();
            GUI::Checkbox("Active", &active);
            pPointLight->GetGameObject()->SetActive(active);
            auto intensity = pPointLight->GetIntensity();
            GUI::SliderFloat("Point Intensity", &intensity, 0.0f, 1.0f);
            pPointLight->SetIntensity(intensity);
        }
        GUI::End();
    }

private:
    GameObject * pLightGO;
    GameObject * pPointLightGO;
    Light * pLight;
    Light * pPointLight;
    
    Vector3 pointLightPos;
    GameObject * pWhiteLittleCube;
};