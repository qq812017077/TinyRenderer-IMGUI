#pragma once
#include <unordered_map>
#include "core/Singleton.h"
#include "components/Renderer.h"
#include "Material.h"

// TODO: add priority to material(like render queue in unity)
class MaterialManager : public Singleton<MaterialManager>
{
    friend class Material;
public:
    MaterialManager();
    ~MaterialManager();
    
    static void AddMaterial(std::shared_ptr<Material> pMaterial);

private:
    std::unordered_map<std::shared_ptr<Material>,unsigned int> uniqueCodeByMaterial;
};