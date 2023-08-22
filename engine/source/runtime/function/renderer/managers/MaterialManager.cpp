#include "MaterialManager.h"
#include <filesystem>
namespace fs = std::filesystem;

MaterialManager::MaterialManager()
{
    uniqueCodeByMaterial = std::unordered_map<std::shared_ptr<Material>,unsigned int>();
}

MaterialManager::~MaterialManager()
{
    uniqueCodeByMaterial.clear();
}

void MaterialManager::AddMaterial(std::shared_ptr<Material> pMaterial)
{
    auto& CodeByMaterial = MaterialManager::Get().uniqueCodeByMaterial;
    auto code = pMaterial->GetUniqueCode();
    CodeByMaterial[pMaterial] = code;
    return;
}
