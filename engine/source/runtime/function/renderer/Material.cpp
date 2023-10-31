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

// 12 bit- 111111111111
#define ID_LENGTH 12
#define RENDERING_QUEUE_LENGTH 13
#define RENDERING_QUEUE_SHIFT ID_LENGTH
uint64_t const_ID_MASK = (1 << ID_LENGTH) - 1;
#define ID_MASK const_ID_MASK
uint64_t const_RENDERING_QUEUE_MASK = ((1 << (RENDERING_QUEUE_LENGTH + ID_LENGTH)) - 1) & (~ID_MASK);
#define RENDERING_QUEUE_MASK const_RENDERING_QUEUE_MASK

std::shared_ptr<Material> Material::pDefaultStandardMaterial = nullptr;
std::shared_ptr<Material> Material::pDefaultCutoutMaterial = nullptr;
std::shared_ptr<Material> Material::pDefaultTransparentMaterial = nullptr;


Material::Material(const Material& mat)
{
    this->pEffect = mat.pEffect;
    this->materialName = mat.materialName;
    instanceId |= MaterialManager::GetNextID() & ID_MASK;
    this->variableMap = mat.variableMap;
    this->queuePriority = mat.queuePriority;
    this->shaderResourceMap = mat.shaderResourceMap;
}

Material::Material(std::shared_ptr<Effect> pEffect, std::string materialName, size_t mat_id)
{
    this->pEffect = pEffect;
    this->materialName = materialName;
    instanceId &= ~ID_MASK;
    instanceId |= mat_id & ID_MASK;
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
/*                  Effect operation                     */
/*********************************************************/
void Material::SetupEffect(std::string effectName)
{
    pEffect = TinyEngine::EffectManager::Get().FindEffect(effectName);
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

void Material::SetRenderQueuePriority(uint16_t priority)
{
    queuePriority = priority;
    instanceId &= ~RENDERING_QUEUE_MASK;
    instanceId |= (queuePriority << RENDERING_QUEUE_SHIFT) & RENDERING_QUEUE_MASK;
}



/*********************************************************/
/*                  uniform buffer operation             */
/*********************************************************/

std::shared_ptr<ShaderResource> Material::GetUpdatedShaderResourcePtr(ShaderBase* shader)
{
    // get shader resource
    std::shared_ptr<ShaderResource> pShaderResource = nullptr;
    if(shaderResourceMap.find(shader) == shaderResourceMap.end())
    {
        pShaderResource = shader->CreateShaderResource();
        shaderResourceMap.emplace(shader, pShaderResource);
    }else
        pShaderResource = shaderResourceMap[shader];
    
    // update shader resource
    for(auto & pair : pShaderResource->variables)
    {
        auto bufferVariable = pair.second;
        if(variableMap.find(pair.first) == variableMap.end()) continue;

        auto & varName = pair.first;
        auto & variable = variableMap[pair.first];
        if(variable.IsDirty())
        {
            switch (variable.GetType())
            {
            case EVarType::Integer:
                bufferVariable->SetInt(variable);
                break;
            case EVarType::Float:
                bufferVariable->SetFloat(variable);
                break;
            case EVarType::Color:
                bufferVariable->SetColor(variable);
                break;
            case EVarType::Matrix4x4:
                bufferVariable->SetMatrix(variable);
                break;
            }
        }
    }
    
    for(auto & pair : pShaderResource->textures)
    {
        auto TexVariable = pair.second;
        if(variableMap.find(pair.first) == variableMap.end()) continue;

        auto & varName = pair.first;
        auto & variable = variableMap[pair.first];
        if(variable.IsDirty())
        {
            switch (variable.GetType())
            {
            case EVarType::Texture:
                TextureInfo& info = variable;
                TexVariable->Set(varName, info.pTexture, info.samplerName);
                break;
            }
        }
    }
    return pShaderResource;
}


/*********************************************************/
/*                set/get operation                      */
/*********************************************************/
void Material::SetInteger(const char * name, int value)
{
    variableMap[name] = value;
}

void Material::SetFloat(const char * name, float value)
{
    variableMap[name] = value;
}
void Material::SetColor(const char * name, Color value)
{
    variableMap[name] = value;
}
void Material::SetMatrix(const char * name, Matrix4x4& value)
{
    variableMap[name] = value;
}

void Material::SetTexture(const char * name, std::shared_ptr<Texture> pTexture, const char * samplerName)
{
    TextureInfo info;
    info.pTexture = pTexture;
    if(samplerName == nullptr)
        info.samplerName = "sampler" + std::string(name);
    else
        info.samplerName = samplerName;
    variableMap[name] = info;
}

bool Material::GetInteger(const char * name, int* result)
{
    // if(pPixelShader != nullptr)
    // {
    //     auto & pVar = pPixelShader->GetVariableByName(name);
    //     if(pVar != nullptr)
    //         return pVar->GetRaw(result, 0, sizeof(int));
    // }

    // if(pVertexShader != nullptr)
    // {
    //     auto & pVar = pVertexShader->GetVariableByName(name);
    //     if(pVar != nullptr)
    //         return pVar->GetRaw(result, 0, sizeof(int));
    // }
    return false;
}

bool Material::GetFloat(const char * name, float* result)
{
    // if(pPixelShader != nullptr)
    // {
    //     auto & pVar = pPixelShader->GetVariableByName(name);
    //     if(pVar != nullptr)
    //         return pVar->GetRaw(result, 0, sizeof(float));
    // }
    
    // if(pVertexShader != nullptr)
    // {
    //     auto & pVar = pVertexShader->GetVariableByName(name);
    //     if(pVar != nullptr)
    //         return pVar->GetRaw(result, 0, sizeof(float));
    // }
    return false;
}

bool Material::GetMatrix4x4(const char * name, float* result, int size)
{
    // if(pPixelShader != nullptr)
    // {
    //     auto & pVar = pPixelShader->GetVariableByName(name);
    //     if(pVar != nullptr)
    //         return pVar->GetRaw(&result, 0, size);
    // }
    // if(pVertexShader != nullptr)
    // {
    //     auto & pVar = pVertexShader->GetVariableByName(name);
    //     if(pVar != nullptr)
    //         return pVar->GetRaw(&result, 0, size);
    // }
    return false;
}

Texture* Material::GetTexturePtr(const char * name)
{
    if (variableMap.find(name) == variableMap.end())
        return Texture::GetDefaultTexturePtr();
    
    TextureInfo & texInfo = variableMap[name];
    return texInfo.pTexture.get();

    // if(pPixelShader != nullptr)
    // {
    //     auto & pTex = pPixelShader->GetTextureByName(name);
    //     if(pTex != nullptr)
    //         return pTex->GetTexturePtr();
    // }
    // if(pVertexShader != nullptr)
    // {
    //     auto & pTex = pVertexShader->GetTextureByName(name);
    //     if(pTex != nullptr)
    //         return pTex->GetTexturePtr();
    // }
    return nullptr;
}

std::string Material::GetSamplerNameByTexName(std::string& texName) const
{
    if (variableMap.find(texName) == variableMap.end())
        return "sampler" + texName;
    TextureInfo texInfo = variableMap.at(texName);
    return texInfo.samplerName;
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


std::shared_ptr<Material> Material::CreateDefault(std::string materialName, ERenderingMode renderingMode)
{
    std::shared_ptr<TinyEngine::Effect> pEffect = TinyEngine::EffectManager::Get().FindDefaultEffect(renderingMode);
    if(pEffect == nullptr || !pEffect->IsValid())
        throw EngineException(__LINE__, __FILE__, "Effect is not valid");

    if(materialName == "") materialName = "New Material (" + pEffect->GetName() + ")";
    int i = 1;
    while(MaterialManager::HasExist(materialName)) materialName = materialName + " (" + std::to_string(i++) + ")";
    auto pMat = Create(pEffect, materialName);
    pMat->SetColor("color", Color::LightGray());
    return pMat;
}

std::shared_ptr<Material> Material::Create(std::string effectName, std::string materialName)
{
    auto pEffect = TinyEngine::EffectManager::Get().FindEffect(effectName);
    if(pEffect == nullptr || !pEffect->IsValid())
        throw EngineException(__LINE__, __FILE__, "Effect is not valid");
    
    if(materialName == "") materialName = "New Material (" + effectName + ")";
    int i = 1;
    while(MaterialManager::HasExist(materialName)) materialName = materialName + " (" + std::to_string(i++) + ")";

    return Create(pEffect, materialName);
}

std::shared_ptr<Material> Material::Create(std::shared_ptr<Effect> pEffect, std::string materialName)
{
    auto pMat = std::shared_ptr<Material>(new Material(pEffect, materialName, MaterialManager::GetNextID()));
    MaterialManager::AddMaterial(pMat);
    return pMat;
}

std::shared_ptr<Material> Material::GetDefaultMaterialPtr(ERenderingMode renderingMode)
{
    switch (renderingMode)
    {
    case ERenderingMode::Opaque:
        if (pDefaultStandardMaterial == nullptr) 
            pDefaultStandardMaterial = CreateDefault("Default-Opaque-Material", ERenderingMode::Opaque);
        return pDefaultStandardMaterial;
        break;
    case ERenderingMode::Cutout:
        if (pDefaultCutoutMaterial == nullptr) 
            pDefaultCutoutMaterial = CreateDefault("Default-Cutout-Material", ERenderingMode::Cutout);
        return pDefaultCutoutMaterial;
        break;
    case ERenderingMode::Transparent:
        if (pDefaultTransparentMaterial == nullptr) 
            pDefaultTransparentMaterial = CreateDefault("Default-Transparent-Material", ERenderingMode::Transparent);
        return pDefaultTransparentMaterial;
        break;
    default:
        break;
    }
    
    if (pDefaultStandardMaterial == nullptr) pDefaultStandardMaterial = CreateDefault("Default-Material", ERenderingMode::Opaque);
    return pDefaultStandardMaterial;
}
/*********************************************************/
