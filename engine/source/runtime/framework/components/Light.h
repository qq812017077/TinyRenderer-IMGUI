#pragma once
#include "components/Component.h"
#include "EngineMath.h"
#include "core/Singleton.h"
#include <memory>
#include <vector>
#include <unordered_map>

class IShaderHelper;
class GameObject;
class Light : public Component
{
public:
    enum Type
    {
        Directional,
        Point,
        Spot
    };
    Light();
    ~Light();

    Vector4 GetColor() const { return Vector4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f) * intensity; }
    void GetLightVP(Matrix4x4 * vpMatrix) const;
    void GetLightView(Matrix4x4 * outputView) const;
    void GetLightProj(Matrix4x4 * outputProj) const;
    void SetIntensity(float intensity) { this->intensity = intensity; }
    void SetRange(float range) { this->range = range; }
    float GetIntensity() { return intensity; }
    float GetRange() { return range; }
    static GameObject* CreateDirectionalLight(std::string name = "Directional Light");
    static GameObject* CreatePointLight(std::string name = "Point Light");
    static GameObject* CreateSpotLight(std::string name = "Spot Light");
    static Light * GetDirectionalLight();
    static Light * GetPointLight();
    static std::vector<Light *> GetPointLightList();
    static Light * GetSpotLight();
protected:
    void Init() override;
private:
    struct DirectionalLight;
    struct PointLight;
    struct SpotLight;

    Type type;
    ColorRGBA color;
    float intensity;
    float range;    // for point light, it is the radius of the sphere
                    // for spot light, it is the distance from the apex to the base of the cone
    float spotAngle; // spot light only: 1 - 179

    static std::unordered_map<Light*, bool> dirLightMap;
    static std::unordered_map<Light*, bool> pointLightMap;
    static std::unordered_map<Light*, bool> spotLightMap;
};
