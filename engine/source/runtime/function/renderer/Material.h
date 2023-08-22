#pragma once
#include <functional>
#include <string>
#include <map>
#include <memory>
#include "UniformBuffer.h"
#include "Texture.h"
class Graphics;
class VertexShader;
class PixelShader;
class Renderer;
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
    Material(std::string vertexShaderPath, std::string pixelShaderPath, std::string materialName);
public:
    ~Material();
    
    int GetUniqueCode() const;
    std::string GetVertexShaderPath() const;
    std::string GetPixelShaderPath() const;
    
    void SetVertexShader(const std::shared_ptr<VertexShader>& pVertexShader);
    void SetPixelShader(const std::shared_ptr<PixelShader>& pPixelShader);
    std::shared_ptr<VertexShader> GetVertexShader() const;
    std::shared_ptr<PixelShader> GetPixelShader() const;

    // Renderer 
    void Bind(Renderer * pRenderer);
    void UnBind(Renderer * pRenderer);
    // uniform buffer operation
    UniformBuffer GetVertexUniformBuffer() const;
    UniformBuffer GetPixelUniformBuffer() const;

    void SetInteger(std::string& name, int value);
    void SetFloat(std::string& name, float value);
    void SetMatrix(std::string& name, float* value);
    void SetTexture(std::string name, std::shared_ptr<Texture> pTexture, std::string samplerName="");

    int GetInteger(std::string& name);
    float GetFloat(std::string& name);
    float* GetMatrix(std::string& name);
    Texture* GetTexturePtr(std::string& name);
    
    std::string GetSamplerNameByTexName(std::string& texName) const;
    static int GetRefCount(std::shared_ptr<Material> pMaterial);
    static std::shared_ptr<Material> CreateInstance(std::shared_ptr<Material> pMaterial);
    static std::shared_ptr<Material> Load(std::string vertexShaderPath, std::string pixelShaderPath);
    static std::shared_ptr<Material> GetDefaultMaterialPtr();
private:
    // if loaded shader, return true
    bool HasLoadedShader() const;
    void LoadShader(Graphics& gfx);

    // uniform buffer operation
    void ClearUniformBuffer();
    void UpdateUniformBuffer();

    std::string materialName;
    std::shared_ptr<VertexShader> pVertexShader;
    std::shared_ptr<PixelShader> pPixelShader;
    std::string vertexShaderPath;
    std::string pixelShaderPath;
    UniformBuffer vertexUniformBuffer;
    UniformBuffer pixelUniformBuffer;
    
    std::map<std::string, TextureInfo> textureMap;
    std::map<std::string, int> integerMap;
    std::map<std::string, float> floatMap;
    std::map<std::string, float*> matrixMap;
    std::unordered_map<Renderer*, bool> rendererRefCountMap;
    unsigned int uniqueCode;
    static std::shared_ptr<Material> pDefaultMaterial;
};

