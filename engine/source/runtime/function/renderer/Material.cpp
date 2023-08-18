#include "Material.h"
#include "MaterialManager.h"
#include "Shader.h"
#include "Graphics.h"
#include "core/EngineException.h"
std::shared_ptr<Material> Material::pDefaultMaterial = nullptr;

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



/*********************************************************/
/*                  shader operation                     */
/*********************************************************/
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



/*********************************************************/
/*                  uniform buffer operation             */
/*********************************************************/
void Material::ClearUniformBuffer()
{
    vertexUniformBuffer.Clear();
    pixelUniformBuffer.Clear();
}

UniformBuffer Material::GetVertexUniformBuffer() const
{
    return vertexUniformBuffer;
}

UniformBuffer Material::GetPixelUniformBuffer() const
{
    return pixelUniformBuffer;
}
void Material::UpdateUniformBuffer()
{
    // if(pVertexShader != nullptr) pVertexShader->updateMaterialUniformBuffer(vertexUniformBuffer);
    // if (pPixelShader != nullptr) pPixelShader->updateMaterialUniformBuffer(pixelUniformBuffer);
}

/*********************************************************/
/*                set/get operation                      */
/*********************************************************/
void Material::SetInteger(std::string& name, int value)
{
    integerMap[name] = value;
}

void Material::SetFloat(std::string& name, float value)
{
    floatMap[name] = value;
}

void Material::SetMatrix(std::string& name, float* value)
{
    matrixMap[name] = value;
}

void Material::SetTexture(std::string name, std::shared_ptr<Texture> pTexture, std::string samplerName)
{
    TextureInfo info;
    info.pTexture = pTexture;
    if(samplerName == "")
        info.samplerName = "sampler" + name;
    else
        info.samplerName = samplerName;
    textureMap[name] = info;
}

int Material::GetInteger(std::string& name)
{
    return integerMap[name];
}

float Material::GetFloat(std::string& name)
{
    return floatMap[name];
}

float* Material::GetMatrix(std::string& name)
{
    return matrixMap[name];
}

Texture* Material::GetTexturePtr(std::string& name)
{
    if (textureMap.find(name) == textureMap.end())
        return Texture::GetDefaultTexturePtr();
    return textureMap[name].pTexture.get();
}

std::string Material::GetSamplerNameByTexName(std::string& texName) const
{
    if (textureMap.find(texName) == textureMap.end())
        return "sampler" + texName;
    return textureMap.at(texName).samplerName;
}
/*********************************************************/
/*                  static function                      */
/*********************************************************/
std::shared_ptr<Material> Material::GetDefaultMaterialPtr()
{
    if (pDefaultMaterial == nullptr)
    {
        std::string vertexShaderPath = "shaders/DefaultVertexShader.hlsl";
        std::string pixelShaderPath = "shaders/DefaultPixelShader.hlsl";
        if(!MaterialManager::ExistMaterial(vertexShaderPath, pixelShaderPath))
            throw EngineException(__LINE__, __FILE__, "Default material not exist");
        
        auto code = MaterialManager::UniqueCode(vertexShaderPath, pixelShaderPath);
        if(!MaterialManager::HasGenerateMaterial(code))
        {
            pDefaultMaterial = std::shared_ptr<Material>(new Material(vertexShaderPath, pixelShaderPath));
            MaterialManager::AddMaterial(pDefaultMaterial);
            MaterialManager::AddCode(code, pDefaultMaterial);
        }
    }
    return pDefaultMaterial;
}
std::shared_ptr<Material> Material::Load(std::string vertexShaderPath, std::string pixelShaderPath)
{
    if(!MaterialManager::ExistMaterial(vertexShaderPath, pixelShaderPath))
        return Material::GetDefaultMaterialPtr();

    auto code = MaterialManager::UniqueCode(vertexShaderPath, pixelShaderPath);
    
    if(MaterialManager::HasGenerateMaterial(code))
        return MaterialManager::GetMaterialByCode(code);

    std::shared_ptr<Material> pMaterial(new Material(vertexShaderPath, pixelShaderPath));
    MaterialManager::AddMaterial(pMaterial);
    MaterialManager::AddCode(code, pMaterial);
    return pMaterial;
    
}

/*********************************************************/
