#include "Light.h"
#include "object/GameObject.h"
#include "Shader.h"
#include "ui/GUI.h"
#include "core/EngineException.h"
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

void Light::GetLightVP(Matrix4x4 * vpMatrix) const
{
    if(type == Type::Directional)
    {
        auto position = pTransform->GetPosition();
        auto rotation = pTransform->GetRotation();

        Matrix4x4 translation = Matrix4x4::Translation(-position);
        Matrix4x4 Rotation = Matrix4x4::Rotation(rotation).Transpose(); // we want inverse, and for orth matrix : transpose = inverse
        auto view = Rotation * translation;
        auto heightSize = 200.0f;
        auto aspect = 1280.0f / 720.0f;
        auto widthSize = heightSize * aspect;
        // light projection matrix
        Matrix4x4 projection = Matrix4x4::OrthographicLH(-widthSize, widthSize, -heightSize, heightSize, 0.1f, 500.0f);
        // projection = Matrix4x4::Perspective(60, 1280.0f / 720.0f, 0.1f, 3000.0f);;

        *vpMatrix = projection * view;
    }else if(type == Type::Point)
    {
        // get six view matrix
        auto position = pTransform->GetPosition();
        Matrix4x4 translation = Matrix4x4::Translation(-position);
        auto up = Vector3(0, 1, 0);
        auto right = Vector3(1, 0, 0);
        auto forward = Vector3(0, 0, 1);
        Matrix4x4 view[6];
        view[0] = Matrix4x4::LookAtLH(position, position + Vector3::right, Vector3::up);
        view[1] = Matrix4x4::LookAtLH(position, position - Vector3::right, Vector3::up);
        view[2] = Matrix4x4::LookAtLH(position, position + Vector3::up, -Vector3::forward);
        view[3] = Matrix4x4::LookAtLH(position, position - Vector3::up, Vector3::forward);
        view[4] = Matrix4x4::LookAtLH(position, position + Vector3::forward, Vector3::up);
        view[5] = Matrix4x4::LookAtLH(position, position - Vector3::forward, Vector3::up);
        
        // get projection matrix
        auto aspect = 1.0f;
        auto fov = 90.0f;
        auto nearPlane = 0.1f;
        auto farPlane = 1000.0f;
        Matrix4x4 projection = Matrix4x4::Perspective(fov, aspect, std::min(0.1f,range), range);

        // get view projection matrix
        for(int i = 0; i < 6; ++i)
            vpMatrix[i] = projection * view[i];
    }else if(type == Type::Spot)
    {
        THROW_ENGINE_EXCEPTION("Spot light is not implemented yet.");
    }
}


void Light::GetLightView(Matrix4x4 * outputView) const 
{
    if(type == Type::Directional)
    {
        auto position = pTransform->GetPosition();
        auto rotation = pTransform->GetRotation();

        Matrix4x4 translation = Matrix4x4::Translation(-position);
        Matrix4x4 Rotation = Matrix4x4::Rotation(rotation).Transpose(); // we want inverse, and for orth matrix : transpose = inverse
        *outputView = Rotation * translation;
    }else if(type == Type::Point)
    {
        // get six view matrix
        auto position = pTransform->GetPosition();
        outputView[0] = Matrix4x4::LookAtLH(position, position + Vector3::right, Vector3::up);
        outputView[1] = Matrix4x4::LookAtLH(position, position - Vector3::right, Vector3::up);
        outputView[2] = Matrix4x4::LookAtLH(position, position + Vector3::up, -Vector3::forward);
        outputView[3] = Matrix4x4::LookAtLH(position, position - Vector3::up, Vector3::forward);
        outputView[4] = Matrix4x4::LookAtLH(position, position + Vector3::forward, Vector3::up);
        outputView[5] = Matrix4x4::LookAtLH(position, position - Vector3::forward, Vector3::up);
    }else if(type == Type::Spot)
    {
        THROW_ENGINE_EXCEPTION("Spot light is not implemented yet.");
    }
}
void Light::GetLightProj(Matrix4x4 * outputProj) const
{
    if(type == Type::Directional)
    {
        auto heightSize = 200.0f;
        auto aspect = 1280.0f / 720.0f;
        auto widthSize = heightSize * aspect;
        *outputProj = Matrix4x4::OrthographicLH(-widthSize, widthSize, -heightSize, heightSize, 0.1f, 500.0f);
    }else if(type == Type::Point)
    {
        // get six view matrix
        auto aspect = 1.0f;
        auto fov = 90.0f;
        auto nearPlane = 0.1f;
        auto farPlane = 1000.0f;
        *outputProj = Matrix4x4::Perspective(fov, aspect, std::min(0.1f,range), range);
    }else if(type == Type::Spot)
    {
        THROW_ENGINE_EXCEPTION("Spot light is not implemented yet.");
    }
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


std::vector<Light *> Light::GetPointLightList()
{
    std::vector<Light *> pointLights;
    for (auto& pair : pointLightMap)
    {
        if(pair.first && pair.first->GetGameObject()->IsActived())
        {
            pointLights.push_back(pair.first);
        }
    }
    return pointLights;
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
