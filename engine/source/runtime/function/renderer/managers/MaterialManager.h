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
    static bool HasExist(std::string& name);
    static size_t GetNextID();
private:
    std::unordered_map<size_t, std::shared_ptr<Material>> MaterialByInstanceID;
    size_t globalMaterialID = 0;
};