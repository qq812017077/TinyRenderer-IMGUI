#include "Material.h"
#include "managers/MaterialManager.h"
#include "Shader.h"
#include "Graphics.h"
#include "core/EngineException.h"
#include "Tools.h"
#include <filesystem>
#include "components/Renderer.h"
#include "core/math/Matrix.h"
namespace fs = std::filesystem;

std::shared_ptr<Material> Material::pDefaultMaterial = nullptr;

static bool ExistShader(std::string vertexShaderPath, std::string pixelShaderPath)
{
    if (vertexShaderPath == "" || pixelShaderPath == "")
        return false;
    // check file exist
    if (!fs::exists(vertexShaderPath) || !fs::exists(pixelShaderPath))
        return false;
    return true;
}

Material::Material(std::string vertexShaderPath, std::string pixelShaderPath, std::string materialName, size_t mat_id)
{
    this->vertexShaderPath = vertexShaderPath;
    this->pixelShaderPath = pixelShaderPath;
    // set name is vertexShaderPath + pixelShaderPath
    this->materialName = materialName;
    instanceId = mat_id;
    Graphics::AddMaterial(this);
}
Material::Material(const Material& mat)
{
    this->vertexShaderPath = mat.vertexShaderPath;
    this->pixelShaderPath = mat.pixelShaderPath;
    this->materialName = mat.materialName;
    instanceId = MaterialManager::GetNextID();
    Graphics::AddMaterial(this);
}

Material::~Material()
{
    // remove material from material manager
}

size_t Material::GetInstanceID() const
{
    return instanceId;
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

void Material::UpdateBuffer()
{
    if(pPixelShader== nullptr || pVertexShader== nullptr) return ;
    for(auto & pair: integerMap)
    {
        pPixelShader->SetInteger(pair.first.c_str(), pair.second);
        pVertexShader->SetInteger(pair.first.c_str(), pair.second);
    }

    for(auto & pair: floatMap)
    {
        pPixelShader->SetFloat(pair.first.c_str(), pair.second);
        pVertexShader->SetFloat(pair.first.c_str(), pair.second);
    }

    for(auto & pair: colorMap)
    {
        pPixelShader->SetColor(pair.first.c_str(), pair.second);
        pVertexShader->SetColor(pair.first.c_str(), pair.second);
    }
    for(auto & pair: matrixMap)
    {
        pPixelShader->SetMatrix(pair.first.c_str(), pair.second);
        pVertexShader->SetMatrix(pair.first.c_str(), pair.second);
    }

    for(auto & pair: textureMap)
    {
        auto pTex = pPixelShader->GetTextureByName(pair.first.c_str());
        if(pTex != nullptr)
            pTex->Set(pair.first, pair.second.pTexture, pair.second.samplerName);

        pTex = pVertexShader->GetTextureByName(pair.first.c_str());
        if(pTex != nullptr)
            pTex->Set(pair.first, pair.second.pTexture, pair.second.samplerName);
    }
    
    integerMap.clear();
    floatMap.clear();
    matrixMap.clear();
    textureMap.clear();
    colorMap.clear();
}

/*********************************************************/
/*                set/get operation                      */
/*********************************************************/
void Material::SetInteger(const char * name, int value)
{
    integerMap[name] = value;
}

void Material::SetFloat(const char * name, float value)
{
    floatMap[name] = value;
}
void Material::SetColor(const char * name, Color value)
{
    colorMap[name] = value;
}
void Material::SetMatrix(const char * name, Matrix4x4& value)
{
    matrixMap[name] = value;
}

void Material::SetTexture(const char * name, std::shared_ptr<Texture> pTexture, const char * samplerName)
{
    TextureInfo info;
    info.pTexture = pTexture;
    if(samplerName == nullptr)
        info.samplerName = "sampler" + std::string(name);
    else
        info.samplerName = samplerName;
    textureMap[name] = info;
}

bool Material::GetInteger(const char * name, int* result)
{
    if(pPixelShader != nullptr)
    {
        auto & pVar = pPixelShader->GetVariableByName(name);
        if(pVar != nullptr)
            return pVar->GetRaw(result, 0, sizeof(int));
    }
    if(pVertexShader != nullptr)
    {
        auto & pVar = pVertexShader->GetVariableByName(name);
        if(pVar != nullptr)
            return pVar->GetRaw(result, 0, sizeof(int));
    }
    return false;
}

bool Material::GetFloat(const char * name, float* result)
{
    if(pPixelShader != nullptr)
    {
        auto & pVar = pPixelShader->GetVariableByName(name);
        if(pVar != nullptr)
            return pVar->GetRaw(result, 0, sizeof(float));
    }
    
    if(pVertexShader != nullptr)
    {
        auto & pVar = pVertexShader->GetVariableByName(name);
        if(pVar != nullptr)
            return pVar->GetRaw(result, 0, sizeof(float));
    }
    return false;
}

bool Material::GetMatrix4x4(const char * name, float* result, int size)
{
    if(pPixelShader != nullptr)
    {
        auto & pVar = pPixelShader->GetVariableByName(name);
        if(pVar != nullptr)
            return pVar->GetRaw(&result, 0, size);
    }
    if(pVertexShader != nullptr)
    {
        auto & pVar = pVertexShader->GetVariableByName(name);
        if(pVar != nullptr)
            return pVar->GetRaw(&result, 0, size);
    }
    return false;
}

Texture* Material::GetTexturePtr(const char * name)
{
    if (textureMap.find(name) == textureMap.end())
        return Texture::GetDefaultTexturePtr();
    return textureMap[name].pTexture.get();

    if(pPixelShader != nullptr)
    {
        auto & pTex = pPixelShader->GetTextureByName(name);
        if(pTex != nullptr)
            return pTex->GetTexturePtr();
    }
    if(pVertexShader != nullptr)
    {
        auto & pTex = pVertexShader->GetTextureByName(name);
        if(pTex != nullptr)
            return pTex->GetTexturePtr();
    }
    return nullptr;
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

int Material::GetRefCount(std::shared_ptr<Material> pMaterial)
{
    return static_cast<int>(pMaterial->rendererRefCountMap.size());
}

std::shared_ptr<Material> Material::CreateInstance(std::shared_ptr<Material> pMaterial)
{
    //Create Instance of material
    auto pNewMaterial = std::shared_ptr<Material>(new Material(*pMaterial));
    MaterialManager::AddMaterial(pNewMaterial);
    return pNewMaterial;
}


std::shared_ptr<Material> Material::CreateDefault(std::string materialName)
{
    std::string vertexShaderPath = "shaders/DefaultVertexShader.hlsl";
    std::string pixelShaderPath = "shaders/DefaultPixelShader.hlsl";

    if(!ExistShader(vertexShaderPath, pixelShaderPath))
        throw EngineException(__LINE__, __FILE__, "Default material not exist");
    // if materialName is exist, make new name
    if(materialName == "")
        materialName = "New Material";
    int i = 1;
    while(MaterialManager::HasExist(materialName)) materialName = "New Material (" + std::to_string(i) + ")";
    auto pMat = Create(vertexShaderPath, pixelShaderPath, materialName);
    pMat->SetColor("color", Color::LightGray());
    return pMat;
}


std::shared_ptr<Material> Material::Create(std::string vertexShaderPath, std::string pixelShaderPath, std::string materialName)
{
    if(!ExistShader(vertexShaderPath, pixelShaderPath))
        return Material::GetDefaultMaterialPtr();
    
    auto vertexShaderFileName = vertexShaderPath.substr(vertexShaderPath.find_last_of("/\\") + 1);
    auto pixelShaderFileName = pixelShaderPath.substr(pixelShaderPath.find_last_of("/\\") + 1);
    vertexShaderFileName = vertexShaderFileName.substr(0, vertexShaderFileName.find_last_of("."));
    pixelShaderFileName = pixelShaderFileName.substr(0, pixelShaderFileName.find_last_of("."));
    if(materialName == "")
        materialName = "Material-VS[" + vertexShaderFileName + "]-PS[" + pixelShaderFileName + "]";
    
    auto pMat = std::shared_ptr<Material>(new Material(vertexShaderPath, pixelShaderPath, materialName, MaterialManager::GetNextID()));
    MaterialManager::AddMaterial(pMat);
    return pMat;
}

std::shared_ptr<Material> Material::GetDefaultMaterialPtr()
{
    if (pDefaultMaterial == nullptr) pDefaultMaterial = CreateDefault("Default-Material");
    return pDefaultMaterial;
}
/*********************************************************/
