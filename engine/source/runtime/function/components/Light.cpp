#include "Light.h"
#include "core/GameObject.h"
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
    if(dirLightMap.empty())
        return nullptr;
    return dirLightMap.begin()->first;
}

Light * Light::GetPointLight()
{
    if(pointLightMap.empty())
        return nullptr;
    return pointLightMap.begin()->first;
}

Light * Light::GetSpotLight()
{
    if(spotLightMap.empty())
        return nullptr;
    return spotLightMap.begin()->first;
}
std::unique_ptr<GameObject> Light::CreateDirectionalLight(std::string name)
{
    auto pGo = std::make_unique<GameObject>(name);
    auto light = pGo->AddComponent<Light>();
    pGo->transform.SetEulerAngle({ 50.0f, -30.0f, 0.0f });
    return pGo;

}
std::unique_ptr<GameObject> Light::CreatePointLight(std::string name)
{
    auto pGo = std::make_unique<GameObject>(name);
    auto light = pGo->AddComponent<Light>();
    light->type = Type::Point;
    light->range = 10.0f;
    dirLightMap.erase(light);
    pointLightMap.emplace(light, true);
    return pGo;
}
std::unique_ptr<GameObject> Light::CreateSpotLight(std::string name)
{
    auto pGo = std::make_unique<GameObject>(name);
    auto light = pGo->AddComponent<Light>();
    light->type = Type::Spot;
    dirLightMap.erase(light);
    spotLightMap.emplace(light, true);
    pGo->transform.SetEulerAngle({ -90.0f, 0.0f, 0.0f });
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
    }
}