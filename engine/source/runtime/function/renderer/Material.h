#pragma once
#include <functional>
#include <string>
#include <memory>
#include "UniformBuffer.h"

class Graphics;
class VertexShader;
class PixelShader;

class Material
{
    friend class Graphics;
private:
    Material() = delete;
    Material(std::string vertexShaderPath, std::string pixelShaderPath);
public:
    ~Material();
    
    std::string GetVertexShaderPath() const;
    std::string GetPixelShaderPath() const;
    
    void SetVertexShader(const std::shared_ptr<VertexShader>& pVertexShader);
    void SetPixelShader(const std::shared_ptr<PixelShader>& pPixelShader);
    std::shared_ptr<VertexShader> GetVertexShader() const;
    std::shared_ptr<PixelShader> GetPixelShader() const;

    static std::shared_ptr<Material> Load(std::string vertexShaderPath, std::string pixelShaderPath);
    
private:
    // TODO: uniform buffer operation
    void ClearUniformBuffer();
    void UpdateUniformBuffer();

    // if loaded shader, return true
    bool HasLoadedShader() const;
    void LoadShader(Graphics& gfx);
    void BindToPipeline();

    std::shared_ptr<VertexShader> pVertexShader;
    std::shared_ptr<PixelShader> pPixelShader;
    std::string vertexShaderPath;
    std::string pixelShaderPath;
    UniformBuffer vertexUniformBuffer;
    UniformBuffer pixelUniformBuffer;
    unsigned int uniqueCode;
};

