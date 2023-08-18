#pragma once
#include <unordered_map>
#include "core/Singleton.h"
#include "components/Renderer.h"

// TODO: add priority to material(like render queue in unity)
class MaterialManager : public Singleton<MaterialManager>
{
    friend class Material;
public:
    MaterialManager();
    ~MaterialManager() = default;
    
    static void AddRenderer(Renderer& renderer);
    static void RemoveRenderer(Renderer& renderer);

    
    static void AddMaterial(std::shared_ptr<Material> pMaterial);
    static void AddCode(unsigned int code, std::shared_ptr<Material> pMaterial);
    static void RemoveMaterial(unsigned int code);
    
    static bool HasGenerateMaterial(std::string vertexShaderPath, std::string pixelShaderPath);
    static bool HasGenerateMaterial(unsigned int uniqueCode);
    static std::unordered_map<std::shared_ptr<Material>,std::vector<std::reference_wrapper<Renderer>>>& GetMatRendererPair();
    static std::shared_ptr<Material>& GetMaterialByCode(unsigned int code);
    static unsigned int UniqueCode(std::string vertexShaderPath, std::string pixelShaderPath);
private:
    static bool ExistMaterial(std::string vertexShaderPath, std::string pixelShaderPath);

    std::unordered_map<std::shared_ptr<Material>,std::vector<std::reference_wrapper<Renderer>>> renderersByMaterial;
    std::unordered_map<unsigned int,std::shared_ptr<Material>> materialByUniqueCode;
    std::unordered_map<std::shared_ptr<Material>,unsigned int> uniqueCodeByMaterial;
};