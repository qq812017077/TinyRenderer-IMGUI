#include "MaterialManager.h"
#include <filesystem>
namespace fs = std::filesystem;

MaterialManager::MaterialManager()
{
    MaterialByInstanceID = std::unordered_map<size_t, std::shared_ptr<Material>>();
}

MaterialManager::~MaterialManager()
{
    MaterialByInstanceID.clear();
}

void MaterialManager::AddMaterial(std::shared_ptr<Material> pMaterial)
{
    auto& matByID = MaterialManager::Get().MaterialByInstanceID;
    auto id = pMaterial->GetInstanceID();
    matByID[id] = pMaterial;
    return;
}

bool MaterialManager::HasExist(std::string& name)
{
    auto& matByID = MaterialManager::Get().MaterialByInstanceID;
    for (auto& [id, pMat] : matByID)
    {
        if (pMat->GetName() == name) return true;
    }
    return false;
}
size_t MaterialManager::GetNextID()
{
    return MaterialManager::Get().globalMaterialID++;
}
