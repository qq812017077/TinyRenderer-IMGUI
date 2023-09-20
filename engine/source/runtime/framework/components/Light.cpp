#include "Light.h"
#include "object/GameObject.h"
#include "Shader.h"
#include "ui/GUI.h"
std::unordered_map<Light*, bool> Light::dirLightMap;
std::unordered_map<Light*, bool> Light::pointLightMap;
std::unordered_map<Light*, bool> Light::spotLightMap;

Light::Light()
{
    type = Type::Directional; 
    color = ColorRGBA(255, 255, 255, 255);
    intensity = 1.0f; 
    range = 10.0f; 
    spotAngle = 30.0f;
    dirLightMap.emplace(this, true);
}

Light::~Light()
{
    if(type == Type::Directional)
        dirLightMap.erase(this);
    else if(type == Type::Point)
        pointLightMap.erase(this);
    else if(type == Type::Spot)
        spotLightMap.erase(this);
}
void Light::Init()
{

}

Light * Light::GetDirectionalLight()
{
    for (auto& pair : dirLightMap)
    {
        if(pair.first && pair.first->GetGameObject()->IsActived())
        {
            return pair.first;
        }
    }
    return nullptr;
}

Light * Light::GetPointLight()
{
    for (auto& pair : pointLightMap)
    {
        if(pair.first && pair.first->GetGameObject()->IsActived())
        {
            return pair.first;
        }
    }
    return nullptr;
}

Light * Light::GetSpotLight()
{
    for (auto& pair : spotLightMap)
    {
        if(pair.first && pair.first->GetGameObject()->IsActived())
        {
            return pair.first;
        }
    }
    return nullptr;
}
GameObject* Light::CreateDirectionalLight(std::string name)
{
    auto pGo = GameObject::CreateGameObject(name);
    auto light = pGo->AddComponent<Light>();
    pGo->transform().SetEulerAngle({ 50.0f, -30.0f, 0.0f });
    return pGo;

}
GameObject* Light::CreatePointLight(std::string name)
{
    auto pGo = GameObject::CreateGameObject(name);
    auto light = pGo->AddComponent<Light>();
    light->type = Type::Point;
    light->range = 10.0f;
    dirLightMap.erase(light);
    pointLightMap.emplace(light, true);
    return pGo;
}
GameObject* Light::CreateSpotLight(std::string name)
{
    auto pGo = GameObject::CreateGameObject(name);
    auto light = pGo->AddComponent<Light>();
    light->type = Type::Spot;
    dirLightMap.erase(light);
    spotLightMap.emplace(light, true);
    pGo->transform().SetEulerAngle({ -90.0f, 0.0f, 0.0f });
    return pGo;
}

struct alignas(16) Light::DirectionalLight
{
    Vector3 dir;
    Vector4 color;
};
struct alignas(16) Light::PointLight
{
    alignas(16) Vector3 pos;
    Vector4 color;
    float atten;
    float range;
    float kConstant;
    float kLinear;
    float kQuadratic;
};
struct alignas(16) Light::SpotLight
{
    alignas(16) Vector3 pos;
    alignas(16) Vector3 dir;
    Vector4 color;
    float range;
    float angle;
};

void Light::UpdateLightBuffer(IShaderHelper& shaderHelper)
{
    auto pDirLight = GetDirectionalLight();
    if(pDirLight != nullptr)
    {
        Light::DirectionalLight dirLight;
        dirLight.dir = pDirLight->pTransform->forward();
        dirLight.color = pDirLight->GetColor();
        shaderHelper.SetGlobalVariable("g_DirLight", &dirLight, sizeof(Light::DirectionalLight));
    }

    auto pPointLight = GetPointLight();
    if(pPointLight != nullptr)
    {
        Light::PointLight pointLight;
        pointLight.pos = pPointLight->pTransform->GetPosition();
        pointLight.color = pPointLight->GetColor();
        pointLight.atten = 1.0f;
        pointLight.range = pPointLight->range;
        shaderHelper.SetGlobalVariable("g_PointLight", &pointLight, sizeof(Light::PointLight));
    }else
    {
        Light::PointLight pointLight;
        pointLight.pos = Vector3(0.0f, 0.0f, 0.0f);
        pointLight.color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
        pointLight.atten = 1.0f;
        pointLight.range = 0.0f;
        shaderHelper.SetGlobalVariable("g_PointLight", &pointLight, sizeof(Light::PointLight));
    }
}