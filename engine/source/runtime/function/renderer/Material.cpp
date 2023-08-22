#include "Material.h"
#include "managers/MaterialManager.h"
#include "Shader.h"
#include "Graphics.h"
#include "core/EngineException.h"
#include "Tools.h"
#include <filesystem>
#include "components/Renderer.h"
namespace fs = std::filesystem;

std::shared_ptr<Material> Material::pDefaultMaterial = nullptr;

static bool ExistMaterial(std::string vertexShaderPath, std::string pixelShaderPath)
{
    if (vertexShaderPath == "" || pixelShaderPath == "")
        return false;
    // check file exist
    if (!fs::exists(vertexShaderPath) || !fs::exists(pixelShaderPath))
        return false;
    return true;
}

Material::Material(std::string vertexShaderPath, std::string pixelShaderPath, std::string materialName)
{
    this->vertexShaderPath = vertexShaderPath;
    this->pixelShaderPath = pixelShaderPath;
    // set name is vertexShaderPath + pixelShaderPath
    this->materialName = "Material-" + materialName;
    uniqueCode = Tools::UniqueCode(this->materialName);
}

Material::~Material()
{
    // remove material from material manager
}

int Material::GetUniqueCode() const
{
    return uniqueCode;
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

// Renderer
void Material::Bind(Renderer * pRenderer)
{
    rendererRefCountMap[pRenderer] = true;
}

void Material::UnBind(Renderer * pRenderer)
{
    //rendererRefCountMap remove pRenderer
    if(rendererRefCountMap.find(pRenderer) == rendererRefCountMap.end())
        return;
    rendererRefCountMap.erase(pRenderer);
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
        if(!ExistMaterial(vertexShaderPath, pixelShaderPath))
            throw EngineException(__LINE__, __FILE__, "Default material not exist");
        
        auto vertexShaderFileName = vertexShaderPath.substr(vertexShaderPath.find_last_of("/\\") + 1);
        auto pixelShaderFileName = pixelShaderPath.substr(pixelShaderPath.find_last_of("/\\") + 1);
        vertexShaderFileName = vertexShaderFileName.substr(0, vertexShaderFileName.find_last_of("."));
        pixelShaderFileName = pixelShaderFileName.substr(0, pixelShaderFileName.find_last_of("."));

        auto materialName = "VS[" + vertexShaderFileName + "]-PS[" + pixelShaderFileName + "]";
        pDefaultMaterial = std::shared_ptr<Material>(new Material(vertexShaderPath, pixelShaderPath, materialName));
        MaterialManager::AddMaterial(pDefaultMaterial);
    }
    return pDefaultMaterial;
}
std::shared_ptr<Material> Material::Load(std::string vertexShaderPath, std::string pixelShaderPath)
{
    if(!ExistMaterial(vertexShaderPath, pixelShaderPath))
        return Material::GetDefaultMaterialPtr();

    // get vertexShader filen name and pixelShader file name and remove postfix
    auto vertexShaderFileName = vertexShaderPath.substr(vertexShaderPath.find_last_of("/\\") + 1);
    auto pixelShaderFileName = pixelShaderPath.substr(pixelShaderPath.find_last_of("/\\") + 1);
    vertexShaderFileName = vertexShaderFileName.substr(0, vertexShaderFileName.find_last_of("."));
    pixelShaderFileName = pixelShaderFileName.substr(0, pixelShaderFileName.find_last_of("."));

    auto materialName = "VS[" + vertexShaderFileName + "]-PS[" + pixelShaderFileName + "]";
    std::shared_ptr<Material> pMaterial(new Material(vertexShaderPath, pixelShaderPath, materialName));
    MaterialManager::AddMaterial(pMaterial);
    return pMaterial;
}

std::shared_ptr<Material> Material::CreateInstance(std::shared_ptr<Material> pMaterial)
{
    //Create Instance of material
    auto pNewMaterial = std::shared_ptr<Material>(new Material(pMaterial->GetVertexShaderPath(), pMaterial->GetPixelShaderPath(),
        pMaterial->materialName + "(Instance)"));
    MaterialManager::AddMaterial(pNewMaterial);
    return pNewMaterial;
}

int Material::GetRefCount(std::shared_ptr<Material> pMaterial)
{
    return static_cast<int>(pMaterial->rendererRefCountMap.size());
}
/*********************************************************/
