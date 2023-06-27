#include "MaterialManager.h"

MaterialManager::MaterialManager()
{
    renderersByMaterial = std::unordered_map<std::shared_ptr<Material>,std::vector<std::reference_wrapper<Renderer>>>();
    materialByUniqueCode = std::unordered_map<unsigned int,std::shared_ptr<Material>>();
    uniqueCodeByMaterial = std::unordered_map<std::shared_ptr<Material>,unsigned int>();
}

void MaterialManager::AddRenderer(Renderer& renderer)
{
    auto pMat = renderer.GetMaterial();
    auto& renderersByMaterial = MaterialManager::Get().renderersByMaterial;
    renderersByMaterial[pMat].push_back(std::ref(renderer));
}

void MaterialManager::RemoveRenderer(Renderer& renderer)
{
    auto pMat = renderer.GetMaterial();
    auto& renderersByMaterial = MaterialManager::Get().renderersByMaterial;
    auto& renderers = renderersByMaterial[pMat];
    auto it = std::find_if(renderers.begin(), renderers.end(), [&renderer](Renderer& r){return &r == &renderer;});
    if(it != renderers.end())
    {
        renderers.erase(it);
    }
}

void MaterialManager::AddMaterial(std::shared_ptr<Material> pMaterial)
{
    auto& renderersByMaterial = MaterialManager::Get().renderersByMaterial;
    renderersByMaterial[pMaterial] = std::vector<std::reference_wrapper<Renderer>>();
    return;
}

void MaterialManager::AddCode(unsigned int code, std::shared_ptr<Material> pMaterial)
{
    auto& materialByCode = MaterialManager::Get().materialByUniqueCode;
    auto& CodeByMaterial = MaterialManager::Get().uniqueCodeByMaterial;
    materialByCode[code] = pMaterial;
    CodeByMaterial[pMaterial] = code;
    return;
}

void MaterialManager::RemoveMaterial(unsigned int uniqueCode)
{
    auto& materialByCode = MaterialManager::Get().materialByUniqueCode;
    auto& CodeByMaterial = MaterialManager::Get().uniqueCodeByMaterial;
    auto& renderersByMaterial = MaterialManager::Get().renderersByMaterial;
    auto pMaterial = materialByCode[uniqueCode];
    materialByCode.erase(uniqueCode);
    CodeByMaterial.erase(pMaterial);
    renderersByMaterial.erase(pMaterial);
    return;
}

bool MaterialManager::ExistMaterial(std::string vertexShaderPath, std::string pixelShaderPath)
{
    auto code = MaterialManager::UniqueCode(vertexShaderPath, pixelShaderPath);
    return ExistMaterial(code);
}

bool MaterialManager::ExistMaterial(unsigned int uniqueCode)
{
    auto& materialByCode = MaterialManager::Get().materialByUniqueCode;
    
    return materialByCode.find(uniqueCode) != materialByCode.end();
}

// std::vector<Renderer&> MaterialManager::GetRenderersByMaterial(std::shared_ptr<Material> pMaterial)
// {
//     auto& renderersByMaterial = MaterialManager::Get().renderersByMaterial;
//     if (renderersByMaterial.find(pMaterial) == renderersByMaterial.end())
//     {
//         return std::vector<Renderer&>();
//     }
//     return renderersByMaterial[pMaterial]; 
// }

unsigned int MaterialManager::UniqueCode(std::string vertexShaderPath, std::string pixelShaderPath)
{
    return static_cast<unsigned int>(std::hash<std::string>{}(vertexShaderPath + pixelShaderPath));
}

std::unordered_map<std::shared_ptr<Material>, std::vector<std::reference_wrapper<Renderer>>>& MaterialManager::GetMatRendererPair()
{
    return MaterialManager::Get().renderersByMaterial;
}

std::shared_ptr<Material>& MaterialManager::GetMaterialByCode(unsigned int code)
{
    auto& materialByCode = MaterialManager::Get().materialByUniqueCode;
    return materialByCode[code];
}