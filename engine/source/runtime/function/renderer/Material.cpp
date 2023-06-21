#include "Material.h"
#include "MaterialManager.h"
#include "Shader.h"
#include "Graphics.h"

Material::Material(std::string vertexShaderPath, std::string pixelShaderPath)
{
    this->vertexShaderPath = vertexShaderPath;
    this->pixelShaderPath = pixelShaderPath;
    uniqueCode = MaterialManager::UniqueCode(vertexShaderPath, pixelShaderPath);
}

Material::~Material()
{
    MaterialManager::Get().RemoveMaterial(uniqueCode);
}

std::string Material::GetVertexShaderPath() const
{
    return vertexShaderPath;
}

std::string Material::GetPixelShaderPath() const
{
    return pixelShaderPath;
}

void Material::SetVertexShader(const std::shared_ptr<VertexShader>& pVertexShader)
{
    this->pVertexShader = pVertexShader;
}

void Material::SetPixelShader(const std::shared_ptr<PixelShader>& pPixelShader)
{
    this->pPixelShader = pPixelShader;
}

std::shared_ptr<VertexShader> Material::GetVertexShader() const
{
    return pVertexShader;
}

std::shared_ptr<PixelShader> Material::GetPixelShader() const
{
    return pPixelShader;
}

bool Material::HasLoadedShader() const
{
    return pVertexShader != nullptr && pPixelShader != nullptr;
}

void Material::LoadShader(Graphics& gfx)
{
    pVertexShader = gfx.CreateVertexShader(vertexShaderPath);
    pPixelShader = gfx.CreatePixelShader(pixelShaderPath);
}

void Material::ClearUniformBuffer()
{
    vertexUniformBuffer.Clear();
    pixelUniformBuffer.Clear();
}
void Material::UpdateUniformBuffer()
{
    if(pVertexShader != nullptr) pVertexShader->updateMaterialUniformBuffer(vertexUniformBuffer);
    if (pPixelShader != nullptr) pPixelShader->updateMaterialUniformBuffer(pixelUniformBuffer);
}

void Material::BindToPipeline()
{
    pVertexShader->bindToPipeline();
    pPixelShader->bindToPipeline();
}

std::shared_ptr<Material> Material::Load(std::string vertexShaderPath, std::string pixelShaderPath)
{
    auto code = MaterialManager::UniqueCode(vertexShaderPath, pixelShaderPath);
    if(!MaterialManager::ExistMaterial(code))
    {
        std::shared_ptr<Material> pMaterial(new Material(vertexShaderPath, pixelShaderPath));
        MaterialManager::AddMaterial(pMaterial);
        MaterialManager::AddCode(code, pMaterial);
        return pMaterial;
    }
    return MaterialManager::GetMaterialByCode(code);
}

/*********************************************************/
