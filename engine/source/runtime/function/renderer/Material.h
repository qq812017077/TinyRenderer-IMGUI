#pragma once
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Texture.h"
class Graphics;
class VertexShader;
class PixelShader;
class Renderer;
struct Matrix4x4;
struct Vector3;
struct Vector4;
struct Color;


class Material
{
    friend class Graphics;
    friend class RenderQueueManager;
private:
    struct TextureInfo
    {
        std::shared_ptr<Texture> pTexture;
        std::string samplerName;
    };

    Material() = delete;
    Material(std::string vertexShaderPath, std::string pixelShaderPath, std::string materialName, size_t mat_id);
    Material(const Material&);
public:
    ~Material();
    
    size_t GetInstanceID() const;
    std::string GetVertexShaderPath() const;
    std::string GetPixelShaderPath() const;
    
    void SetVertexShader(const std::shared_ptr<VertexShader>& pVertexShader);
    void SetPixelShader(const std::shared_ptr<PixelShader>& pPixelShader);
    std::shared_ptr<VertexShader> GetVertexShader() const;
    std::shared_ptr<PixelShader> GetPixelShader() const;

    std::string GetName() const { return materialName; }
    // Renderer 
    void Bind(Renderer * pRenderer);
    void UnBind(Renderer * pRenderer);
    // uniform buffer operation
    void SetInteger(const char * name, int value);
    void SetFloat(const char * name, float value);
    void SetColor(const char * name, Color value);
    void SetMatrix(const char * name, Matrix4x4& value);
    void SetTexture(const char * name, std::shared_ptr<Texture> pTexture, const char * samplerName = nullptr);
    bool GetInteger(const char * name, int* result);
    bool GetFloat(const char * name, float* result);
    bool Material::GetMatrix4x4(const char * name, float* result, int size);
    
    Texture* GetTexturePtr(const char * name);
    
    std::string GetSamplerNameByTexName(std::string& texName) const;
    static int GetRefCount(std::shared_ptr<Material> pMaterial);
    static std::shared_ptr<Material> CreateInstance(std::shared_ptr<Material> pMaterial);
    static std::shared_ptr<Material> CreateDefault(std::string materialName = "New Material");
    static std::shared_ptr<Material> Create(std::string vertexShaderPath, std::string pixelShaderPath, std::string materialName = "");
    static std::shared_ptr<Material> GetDefaultMaterialPtr();
private:
    // if loaded shader, return true
    bool HasLoadedShader() const;
    void LoadShader(Graphics& gfx);

    // uniform buffer operation
    void UpdateBuffer();

    std::string materialName;
    std::shared_ptr<VertexShader> pVertexShader;
    std::shared_ptr<PixelShader> pPixelShader;
    std::string vertexShaderPath;
    std::string pixelShaderPath;

    // std::unordered_map<std::string, Property> propertyMap;
    std::map<std::string, TextureInfo> textureMap;
    std::map<std::string, int> integerMap;
    std::map<std::string, float> floatMap;
    std::map<std::string, Color> colorMap;
    std::map<std::string, Matrix4x4> matrixMap;
    std::unordered_map<Renderer*, bool> rendererRefCountMap;

    size_t instanceId;
    static std::shared_ptr<Material> pDefaultMaterial;
};
